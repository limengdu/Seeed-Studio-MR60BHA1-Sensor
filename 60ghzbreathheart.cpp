#include "Arduino.h"
#include "60ghzbreathheart.h"

BreathHeart_60GHz::Breath_Heart_60GHz(Stream *s)
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
void BreathHeart_60GHz::Situation_judgment(){
  recvRadarBytes();
  sensor_report = 0x00;
  bodysign_val = 0x00;
  distance = 0x00;
  Dir_x = 0x00;
  Dir_y = 0x00;
  Dir_z = 0x00;
  if(this->newData){
    switch(Msg[0]){
      case HUMAN_PSE_RADAR:
        switch(Msg[1]){
          case PRESENCE_INF:
            switch(Msg[4]){
              case NOONE_HERE:
                showData(Msg);
                sensor_report = NOONE;
                // Serial.println("Radar detects no one.");
                // Serial.println("----------------------------");
                break;
              case SOMEONE_HERE:
                showData(Msg);
                sensor_report = SOMEONE;
                // Serial.println("Radar detects somebody.");
                // Serial.println("----------------------------");
                break;
            }
            break;
          case MOVE_INF:
            switch(Msg[4]){
              case PSE_NONE:
                showData(Msg);
                sensor_report = NONEPSE;
                // Serial.println("Radar detects None.");
                // Serial.println("----------------------------");
                break;
              case STATIONARY:
                showData(Msg);
                sensor_report = STATION;
                // Serial.println("Radar detects somebody stationary.");
                // Serial.println("----------------------------");
                break;
              case MOVEMENT:
                showData(Msg);
                sensor_report = MOVE;
                // Serial.println("Radar detects somebody in motion.");
                // Serial.println("----------------------------");
                break;
            }
            break;
          case BODY_SIG:
            showData(Msg);
            sensor_report = BODYVAL;
            bodysign_val = Msg[4];
            // Serial.print("The radar identifies the current motion feature value is: ");
            // Serial.println(Msg[4]);
            // Serial.println("----------------------------");
            break;
          case DISTANCE:
            showData(Msg);
            sensor_report = DISVAL;
            distance = (Msg[4] << 8 | Msg[5]) / 100.0; // Distance in meters
            // Serial.print("The distance of the radar from the monitored person is: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.print(Msg[5]);
            // Serial.println(" cm");
            // Serial.println("----------------------------");
            break;
          case DIRECTIONS:
            showData(Msg);
            sensor_report = DIREVAL;
            Dir_x = Byte2Int(Msg[4], Msg[5]);
            Dir_y = Byte2Int(Msg[6], Msg[7]);
            Dir_z = Byte2Int(Msg[8], Msg[9]);
            // Serial.print("x: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.print(Msg[5]);
            // Serial.print(" ");
            // Serial.print("y: ");
            // Serial.print(Msg[6]);
            // Serial.println(" ");
            // Serial.print(Msg[7]);
            // Serial.println(" ");
            // Serial.print("z: ");
            // Serial.print(Msg[8]);
            // Serial.print(" ");
            // Serial.print(Msg[9]);
            // Serial.println();
            // Serial.println("----------------------------");
            break;
        }
        break;
    }
  }
}

//Respiratory and heart rate data analysis
void BreathHeart_60GHz::Breath_Heart(){
  recvRadarBytes();
  sensor_report = 0x00;
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
  if(this->newData){
    switch(Msg[0]){
      case HEART_INF:
        switch(Msg[1]){
          case HEART_RATE:
            showData(Msg);
            sensor_report = HEARTRATEVAL;
            heart_rate = Msg[4];
            // Serial.print("Radar monitored the current heart rate value is ");
            // Serial.println(Msg[4]);
            // Serial.println("----------------------------");
            break;
          case HEART_RATE_WAVE:
            showData(Msg);
            sensor_report = HEARTRATEWAVE;
            heart_point_1 = Msg[4];
            heart_point_2 = Msg[5];
            heart_point_3 = Msg[6];
            heart_point_4 = Msg[7];
            heart_point_5 = Msg[8];
            // Serial.print("The heart rate waveform: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.print(Msg[5]);
            // Serial.print(" ");
            // Serial.print(Msg[6]);
            // Serial.print(" ");
            // Serial.print(Msg[7]);
            // Serial.print(" ");
            // Serial.println(Msg[8]);
            // Serial.println("----------------------------");
            break;
        }
        break;
      case BREATH_RATE_RADAR:
        switch(Msg[1]){
          case BREATH_INF:
            switch(Msg[4]){
              case BREATH_NORMAL:
                showData(Msg);
                sensor_report = BREATHNOR;
                // Serial.println("Radar detects that the current breath rate is normal.");
                // Serial.println("----------------------------");
                break;
              case BREATH_RAPID:
                showData(Msg);
                sensor_report = BREATHRAPID;
                // Serial.println("Radar detects current breath rate is too fast");
                // Serial.println("----------------------------");
                break;
              case BREATH_SLOW:
                showData(Msg);
                sensor_report = BREATHSLOW;
                // Serial.println("Radar detects current breath rate is too slow");
                // Serial.println("----------------------------");
                break;
              case BREATH_NONE:
                showData(Msg);
                sensor_report = BREATHNONE;
                // Serial.println("None");
                // Serial.println("----------------------------");
                break;
            }
            break;
          case BREATH_VAL:
            showData(Msg);
            sensor_report = BREATHVAL;
            breath_rate = Msg[4];
            // Serial.print("Radar monitored the current breath rate value is ");
            // Serial.println(Msg[4]);
            // Serial.println("----------------------------");
            break;
          case BREATH_WAVE:
            showData(Msg);
            sensor_report = BREATHWAVE;
            breath_point_1 = Msg[4];
            breath_point_2 = Msg[5];
            breath_point_3 = Msg[6];
            breath_point_4 = Msg[7];
            breath_point_5 = Msg[8];
            // Serial.print("The breath rate waveform: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.print(Msg[5]);
            // Serial.print(" ");
            // Serial.print(Msg[6]);
            // Serial.print(" ");
            // Serial.print(Msg[7]);
            // Serial.print(" ");
            // Serial.println(Msg[8]);
            // Serial.println("----------------------------");
            break;
        }
        break;
    }
  }
}

void BreathHeart_60GHz::SleepInf_Decode(){
  recvRadarBytes();
  if(this->newData){
    switch(Msg[0]){
      case SLEEP_INF:
        switch(Msg[1]){
          case INOUT_BED:
            switch(Msg[4]){
              case OUT_BED:
                showData(Msg);
                sensor_report = OUTBED;
                // Serial.println("Radar detects someone currently leaving the bed.");
                // Serial.println("----------------------------");
                break;
              case IN_BED:
                showData(Msg);
                sensor_report = INBED;
                // Serial.println("Radar detects that someone is currently in bed.");
                // Serial.println("----------------------------");
                break;
              case INOUT_NONE:
                showData(Msg);
                sensor_report = NOINOUT;
                // Serial.println("NONE.");
                // Serial.println("----------------------------");
                break;
            }
            break;
          case SLEEP_STATE:
            switch(Msg[4]){
              case AWAKE:
                showData(Msg);
                sensor_report = 
                // Serial.println("The radar detects that the monitoring people is awake.");
                // Serial.println("----------------------------");
                break;
              case LIGHT_SLEEP:
                showData(Msg);
                sensor_report = 
                // Serial.println("The radar detects that the monitoring people is in light sleeping.");
                // Serial.println("----------------------------");
                break;
              case DEEP_SLEEP:
                showData(Msg);
                sensor_report = 
                // Serial.println("The radar detects that the monitoring people is in deep sleeping.");
                // Serial.println("----------------------------");
                break;
              case SLEEP_NONE:
                showData(Msg);
                sensor_report = 
                // Serial.println("The radar detects that the monitoring people nothing.");
                // Serial.println("----------------------------");
                break;
            }
          case AWAKE_TIME:
            showData(Msg);
            sensor_report = 
            // Serial.print("Radar monitored the awake sleep time is: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.println(Msg[5]);
            // Serial.println("----------------------------");
            break;
          case LIGHTSLEEP_TIME:
            showData(Msg);
            sensor_report = 
            // Serial.print("Radar monitored the light sleep time is: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.println(Msg[5]);
            // Serial.println("----------------------------");
            break;
          case DEEPSLEEP_TIME:
            showData(Msg);
            sensor_report = 
            // Serial.print("Radar monitored the deep sleep time is: ");
            // Serial.print(Msg[4]);
            // Serial.print(" ");
            // Serial.println(Msg[5]);
            // Serial.println("----------------------------");
            break;
          case SLEEP_SCORE:
            showData(Msg);
            sensor_report = 
            // Serial.print("Radar judgment sleep score is: ");
            // Serial.println(Msg[4]);
            // Serial.println("----------------------------");
            break;
          case SLEEP_STATUE:
            showData(Msg);
            sensor_report = 
            break;
          case SLEEP_QUALITY:
            showData(Msg);
            sensor_report = 
            break;
          case SLEEP_ERROR:
            showData(Msg);
            sensor_report = 
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
