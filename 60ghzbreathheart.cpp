#include "Arduino.h"
#include "60ghzbreathheart.h"

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3); // RX, TX
    #define Serial1 SSerial
#endif

void BreathHeart_60GHz::SerialInit(){
  Serial1.begin(115200);
}

// Receive data and process
void BreathHeart_60GHz::recvRadarBytes(){
  static boolean recvInProgress = false;
  static byte ndx = 0;
  byte startMarker = MESSAGE_HEAD;            //Header frame
  byte endMarker = MESSAGE_TAIL;
  byte rb; // Each frame received
  while (Serial1.available() > 0 && newData == false)
  {
    rb = Serial1.read();
    if (recvInProgress == true)
    {                     // Received header frame
      if (rb != endMarker){ // Length in range
        Msg[ndx] = rb;
        ndx++;
      }
      else{
        recvInProgress = false;
        dataLen = ndx;
        ndx = 0;
        newData = true;
      }
    }
    else if (rb == startMarker){ // Waiting for the first frame to arrive
        recvInProgress = true;
    }
  }
}

//Radar transmits data frames for display via serial port
void BreathHeart_60GHz::ShowData(byte inf[]){
  for (byte n = 0; n < dataLen+3; n++) {
    Serial.print(inf[n], HEX);
    Serial.print(' ');
  }
    Serial.println();
}


// Judgment of occupied and unoccupied, approach and distance
void BreathHeart_60GHz::Situation_judgment(byte inf[]){
  switch(inf[2]){
    case HUMAN_PSE_RADAR:
      switch(inf[3]){
        case PRESENCE_INF:
          switch(inf[6]){
            case NOONE_HERE:
              ShowData(inf);
              Serial.println("Radar detects no one.");
              Serial.println("----------------------------");
              break;
            case SOMEONE_HERE:
              ShowData(inf);
              Serial.println("Radar detects somebody.");
              Serial.println("----------------------------");
              break;
          }
          break;
        case MOVE_INF:
          switch(inf[6]){
            case NONE:
              ShowData(inf);
              Serial.println("Radar detects None.");
              Serial.println("----------------------------");
              break;
            case CA_CLOSE:
              ShowData(inf);
              Serial.println("Radar detects somebody close.");
              Serial.println("----------------------------");
              break;
            case CA_AWAY:
              ShowData(inf);
              Serial.println("Radar detects somebody away.");
              Serial.println("----------------------------");
              break;
            case DISORDER:
              ShowData(inf);
              Serial.println("Radar detects someone is in disorderly motion.");
              Serial.println("----------------------------");
              break;
          }
          break;
        case BODY_SIG:
          ShowData(inf);
          Serial.print("The radar identifies the current motion feature value is: ");
          Serial.println(inf[6]);
          Serial.println("----------------------------");
          break;
      }
      break;
  }
}

//Respiratory sleep data frame decoding
void BreathHeart_60GHz::Breath_Heart(byte inf[]){
  switch(inf[2]){
    case HEART_RATE_RADAR:
      switch(inf[3]){
        case RATE_DATA:
          switch(inf[6]){
            case RATE_NORMAL:
              ShowData(inf);
              Serial.println("Radar detects that the current heart rate is normal.");
              Serial.println("----------------------------");
              break;
            case RATE_RAPID:
              ShowData(inf);
              Serial.println("Radar detects current heart rate is too fast");
              Serial.println("----------------------------");
              break;
            case RATE_SLOW:
              ShowData(inf);
              Serial.println("Radar detects current heart rate is too slow");
              Serial.println("----------------------------");
              break;
          }
          break;
        case HEART_RATE:
          ShowData(inf);
          Serial.print("Radar monitored the current heart rate value is ");
          Serial.println(inf[6]);
          Serial.println("----------------------------");
          break;
        case HEART_RATE_WAVE:
          ShowData(inf);
          break;
        case BREATH_DATA:
          switch(inf[6]){
            case BREATH_NORMAL:
              ShowData(inf);
              Serial.println("Radar detects that the current breathing rate is normal.");
              Serial.println("----------------------------");
              break;
            case BREATH_RAPID:
              ShowData(inf);
              Serial.println("Radar detects current breathing rate is too fast");
              Serial.println("----------------------------");
              break;
            case BREATH_SLOW:
              ShowData(inf);
              Serial.println("Radar detects current breathing rate is too slow");
              Serial.println("----------------------------");
              break;
          }
          break;
        case BREATH_VAL:
          ShowData(inf);
          Serial.print("Radar monitored the current breathing rate value is ");
          Serial.println(inf[6]);
          Serial.println("----------------------------");
          break;
        case BREATH_WAVE:
          ShowData(inf);
          break;
        case LOCA_DET_ANOMAL:
          switch(inf[6]){
            case OUT_OF_RANGE:
              ShowData(inf);
              Serial.println("Radar detects that the current user is out of monitoring range.");
              Serial.println("----------------------------");
              break;
            case WITHIN_RANGE:
              ShowData(inf);
              Serial.println("Radar detects that the current user is within monitoring range.");
              Serial.println("----------------------------");
              break;
          }
          break;
        case DISTANCE:
          // Distance(inf);
          // break;
          ShowData(inf);
          Serial.print("The current standstill distance detected by the radar is ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.print(inf[7]);
          Serial.println(" CM");
          Serial.println("----------------------------");
          break;
        case ANGLE:
          ShowData(inf);
          Serial.print("The current standstill angle detected by the radar is ");
          Serial.print(inf[6]);
          Serial.print(" ");
          Serial.print(inf[7]);
          Serial.println(" °C");
          Serial.println("----------------------------");
          break;
      }
      break;
  }
}




