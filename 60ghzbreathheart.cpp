#include "Arduino.h"
#include "60ghzbreathheart.h"

BreathHeart_60GHz::BreathHeart_60GHz(Stream *s)
    : stream(s){
  this->newData = false;
}

// Receive data and process
void BreathHeart_60GHz::recvRadarBytes(){
  while (stream->available()) {
    if(stream->read() == MESSAGE_HEAD1){           //Receive header frame 1
      if(stream->read() == MESSAGE_HEAD2){         //Receive header frame 2
        dataLen = stream->readBytesUntil(MESSAGE_END2, Msg, 20);
        if (dataLen > 0 && dataLen < 20){
          Msg[dataLen] = MESSAGE_END2;
          this->newData = true;
        }
      }
    }
  }
}

//Radar transmits data frames for display via serial port
void BreathHeart_60GHz::showData(){
  if(this->newData){
    Serial.print(MESSAGE_HEAD1, HEX);
    Serial.print(' ');
    Serial.print(MESSAGE_HEAD2, HEX);
    Serial.print(' ');
    char charVal[4];
    for (byte n = 0; n < dataLen+1; n++) {
      sprintf(charVal, "%02X", Msg[n]);
      Serial.print(charVal);
      Serial.print(' ');
    }
    Serial.println();
    this->newData = false;
    Msg[dataLen] = {0};
  }
}

// Judgment of occupied and unoccupied, approach and distance
void BreathHeart_60GHz::HumanExis_Func(){
  recvRadarBytes();
  reset_val();
  if(this->newData){
    switch(Msg[0]){
      case HUMAN_PSE_RADAR:
        switch(Msg[1]){
          case PRESENCE_INF:
            switch(Msg[4]){
              case NOONE_HERE:
                showData();
                sensor_report = NOONE;
                break;
              case SOMEONE_HERE:
                showData();
                sensor_report = SOMEONE;
                break;
            }
            break;
          case MOVE_INF:
            switch(Msg[4]){
              case PSE_NONE:
                showData();
                sensor_report = NONEPSE;
                break;
              case STATIONARY:
                showData();
                sensor_report = STATION;
                break;
              case MOVEMENT:
                showData();
                sensor_report = MOVE;
                break;
            }
            break;
          case BODY_SIG:
            showData();
            sensor_report = BODYVAL;
            bodysign_val = Msg[4];
            break;
          case DISTANCE:
            showData();
            sensor_report = DISVAL;
            distance = (Msg[4] << 8 | Msg[5]) / 100.0; // Distance in meters
            break;
          case DIRECTIONS:
            showData();
            sensor_report = DIREVAL;
            Dir_x = Byte2Int(Msg[4], Msg[5]);
            Dir_y = Byte2Int(Msg[6], Msg[7]);
            Dir_z = Byte2Int(Msg[8], Msg[9]);
            break;
        }
        break;
    }
  }
}

//Respiratory and heart rate data analysis
void BreathHeart_60GHz::Breath_Heart(){
  recvRadarBytes();
  reset_val();
  if(this->newData){
    switch(Msg[0]){
      case HEART_INF:
        switch(Msg[1]){
          case HEART_RATE:
            showData();
            sensor_report = HEARTRATEVAL;
            heart_rate = Msg[4];
            break;
          case HEART_RATE_WAVE:
            showData();
            sensor_report = HEARTRATEWAVE;
            heart_point_1 = Msg[4];
            heart_point_2 = Msg[5];
            heart_point_3 = Msg[6];
            heart_point_4 = Msg[7];
            heart_point_5 = Msg[8];
            break;
        }
        break;
      case BREATH_RATE_RADAR:
        switch(Msg[1]){
          case BREATH_INF:
            switch(Msg[4]){
              case BREATH_NORMAL:
                showData();
                sensor_report = BREATHNOR;
                break;
              case BREATH_RAPID:
                showData();
                sensor_report = BREATHRAPID;
                break;
              case BREATH_SLOW:
                showData();
                sensor_report = BREATHSLOW;
                break;
              case BREATH_NONE:
                showData();
                sensor_report = BREATHNONE;
                break;
            }
            break;
          case BREATH_VAL:
            showData();
            sensor_report = BREATHVAL;
            breath_rate = Msg[4];
            break;
          case BREATH_WAVE:
            showData();
            sensor_report = BREATHWAVE;
            breath_point_1 = Msg[4];
            breath_point_2 = Msg[5];
            breath_point_3 = Msg[6];
            breath_point_4 = Msg[7];
            breath_point_5 = Msg[8];
            break;
        }
        break;
    }
  }
}

void BreathHeart_60GHz::SleepInf_Decode(){
  recvRadarBytes();
  reset_val();
  if(this->newData){
    switch(Msg[0]){
      case SLEEP_INF:
        switch(Msg[1]){
          case INOUT_BED:
            switch(Msg[4]){
              case OUT_BED:
                showData();
                sensor_report = OUTBED;
                break;
              case IN_BED:
                showData();
                sensor_report = INBED;
                break;
              case INOUT_NONE:
                showData();
                sensor_report = NOINOUT;
                break;
            }
            break;
          case SLEEP_STATE:
            switch(Msg[4]){
              case AWAKE:
                showData();
                sensor_report = SLEEPAWAKE;
                break;
              case LIGHT_SLEEP:
                showData();
                sensor_report = SLEEPLIGHT;
                break;
              case DEEP_SLEEP:
                showData();
                sensor_report = SLEEPDEEP;
                break;
              case SLEEP_NONE:
                showData();
                sensor_report = SLEEPNONE;
                break;
            }
          case AWAKE_TIME:
            showData();
            sensor_report = AWAKETIME;
            awake_time = Msg[4] << 8 | Msg[5];   // Time: minutes
            break;
          case LIGHTSLEEP_TIME:
            showData();
            sensor_report = LIGHTTIME;
            light_time = Msg[4] << 8 | Msg[5];   // Time: minutes
            break;
          case DEEPSLEEP_TIME:
            showData();
            sensor_report = DEEPTIME;
            deep_time = Msg[4] << 8 | Msg[5];   // Time: minutes
            break;
          case SLEEP_SCORE:
            showData();
            sensor_report = SLEEPSCORE;
            sleep_score = Msg[4];
            break;
          case SLEEP_STATUE:
            showData();
            sensor_report = SLEEPSTATUE;
            switch(Msg[4]){
              case SOMEONE_HERE:
                existence = true;
                break;
              case NOONE_HERE:
                existence = false;
                break;
            }
            switch(Msg[5]){
              case DEEP_SLEEP:
                sleep_status = SLEEPDEEP;
                break;
              case LIGHT_SLEEP:
                sleep_status = SLEEPLIGHT;
                break;
              case AWAKE:
                sleep_status = SLEEPAWAKE;
                break;
              case SLEEP_NONE:
                sleep_status = SLEEPNONE;
                break;
            }
            breath_rate = Msg[6];
            heart_rate = Msg[7];
            turn_num = Msg[8];
            substantial_move_ratio = Msg[9];
            samll_move_ratio = Msg[10];
            apnea_num = Msg[11];
            break;
          case SLEEP_QUALITY:
            showData();
            sensor_report = SLEEPQUALITY;
            sleep_score = Msg[4];
            sleep_time = Msg[5] << 8 | Msg[6];
            awake_time_radio = Msg[7];
            light_time_radio = Msg[8];
            deep_time_radio = Msg[9];
            outbed_time = Msg[10];
            outbed_num = Msg[11];
            turn_num = Msg[12];
            breath_rate = Msg[13];
            heart_rate = Msg[14];
            apnea_num = Msg[15];
            break;
          case SLEEP_ERROR:
            switch(Msg[4]){
              case SLEEP_LESS4H:
                showData();
                sensor_report = SLEEPLESS4H;
                break;
              case SLEEP_OVER12H:
                showData();
                sensor_report = SLEEPOVER12H;
                break;
              case SLEEP_LONGTIMENOONE:
                showData();
                sensor_report = LONGTIMENOONE;
                break;
              case SLEEP_ERRORNONE:
                showData();
                sensor_report = ERRORNONE;
                break;
            }
            break;
        }
        break;
    }
  }
}

//Send data frame
void BreathHeart_60GHz::send_func(const unsigned char* buff, int len, bool cyclic /*=false*/){
  if(cyclic || count < checkdata_len){
    if(cyclic || count < 1){
      stream->write(buff, len);
      stream->flush();
    }
    do{
      recvRadarBytes();
      delay(20);
    }while(!(this->newData));
    if(cyclic || count < 1){
      Serial.print("  Sent  ---> ");
      data_printf(buff, len);
    }
    if(count%2 == 1){
      Serial.print("Receive <--- ");
      showData();
    }
    this->newData = false;
  }
  count++;
}

//Transfer mode selection
void BreathHeart_60GHz::ModeSelect_fuc(int mode){
  if (mode == 1){
    stream->write(realtime_mode_frame, mode_frame_len);
    stream->flush();
    Serial.println("Real-time data transfer mode ON!");
  }
  else if (mode == 2){
    stream->write(sleepstatus_mode_frame, mode_frame_len);
    stream->flush();
    Serial.println("Sleep state transfer mode ON!");
  }
  else Serial.println("Input error, please reselect the mode - 1: indicates real-time transmission mode, 2: indicates sleep state mode.");
}

//Reset radar
void BreathHeart_60GHz::reset_func(){
  stream->write(reset_frame, reset_frame_len);
  stream->flush();
  Serial.println("Radar reset!");
}

//Two Byte to signed integer
float BreathHeart_60GHz::Byte2Int(unsigned int x, unsigned int y){
  float z = ((x << 8 | y) & 0x7FFF) / 100.00;
  if (((x << 8 | y) & 0x8000) == 0x8000)z = -z;
  return z;
}

//print redirect
void BreathHeart_60GHz::data_printf(const unsigned char* buff, int len){
  char charVal[4];
  for(int i=0; i<len; i++){
    sprintf(charVal, "%02X", buff[i]);
    Serial.print(charVal);
    Serial.print(' ');
  }
  Serial.println();
}

//reset the radar values
void BreathHeart_60GHz::reset_val(){
  sensor_report = 0x00;

  bodysign_val = 0x00;
  distance = 0x00;
  Dir_x = 0x00;
  Dir_y = 0x00;
  Dir_z = 0x00;

  heart_rate = 0x00;
  heart_point_1 = 0x00;
  heart_point_2 = 0x00;
  heart_point_3 = 0x00;
  heart_point_4 = 0x00;
  heart_point_5 = 0x00;
  breath_rate = 0x00;
  breath_point_1 = 0x00;
  breath_point_2 = 0x00;
  breath_point_3 = 0x00;
  breath_point_4 = 0x00;
  breath_point_5 = 0x00;

  awake_time = 0x00;
  light_time = 0x00;
  deep_time = 0x00;
  sleep_score = 0x00;
  sleep_status = 0x00;
  turn_num = 0x00;
  substantial_move_ratio = 0x00;
  samll_move_ratio = 0x00;
  apnea_num = 0x00;
  sleep_time = 0x00;
  awake_time_radio = 0x00;
  light_time_radio = 0x00;
  deep_time_radio = 0x00;
  outbed_time = 0x00;
  outbed_num = 0x00;
}
