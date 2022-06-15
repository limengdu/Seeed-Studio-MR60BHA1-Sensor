#ifndef _RADAR_H__
#define _RADAR_H__

#define MESSAGE_HEAD 0x53       //Data frame header
#define MESSAGE_TAIL 0x54       //Data frame tail

#define HUMAN_PSE_RADAR 0x80    //Human presence data

#define PRESENCE_INF 0x01       //Presence Information
#define SOMEONE_HERE 0x01       //Someone here
#define NOONE_HERE 0x00         //Noone here

#define MOVE_INF 0x02           //Campaign Information
#define NONE 0x00               //None
#define STATIONARY 0x01         //A person is stationary
#define MOVEMENT 0x02           //A person in motion

#define BODY_SIG 0x03           //Body movement information

#define DISTANCE 0x04           //Distance from the person being detected

#define ANGLE 0x05              //Body orientation

#define BREATH_RATE_RADAR 0x81  //Respiratory heart rate data

#define BREATH_DATA 0x01        //Breathing data
#define BREATH_NORMAL 0x01      //Normal breathing
#define BREATH_RAPID 0x02       //Acute respiratory abnormalities
#define BREATH_SLOW 0x03        //Slow heartbeat
#define BREATH_DETECTING 0x04   //Radar detection in progress

#define BREATH_VAL 0x02         //Breathing values

#define BREATH_INTENSITY 0x03   //Breathing intensity

#define BREATH_CONFIDENCE 0x04  //Confidence

#define BREATH_WAVE 0x05        //Respiratory waveform (No analysis for now)

#define SLEEP_INF 0x84          //Sleep information

#define INOUT_BED 0x01          //In bed or out of bed
#define OUT_BED 0x00            //Out of bed
#define IN_BED 0x01             //In bed

#define SLEEP_STATE 0x02        //Sleep state
#define AWAKE 0x00              //Awake
#define LIGHT_SLEEP 0x01        //Light sleep
#define DEEP_SLEEP 0x02         //Deep sleep
#define SLEEP_NONE 0x03         //None

#define AWAKE_TIME 0x03         //Awake time

#define LIGHTSLEEP_TIME 0x04    //Light sleep time

#define DEEPSLEEP_TIME 0x05     //Deep sleep time

#define SLEEP_SCORE 0x06        //Sleep quality score

#define HEART_INF 0x85          //Heart rate information

#define RATE_DATA 0x01          //Heartbeat Pack
#define RATE_NORMAL 0x01        //Normal heart rate
#define RATE_RAPID 0x02         //Rapid heart rate
#define RATE_SLOW 0x03          //Slow heart rate
#define RATE_DETECTING 0x04     //Radar detection in progress

#define HEART_RATE 0x02         //Heart rate

#define RATE_INTENSITY 0x03     //Heart intensity

#define RATE_CONFIDENCE 0x04    //Confidence

#define HEART_RATE_WAVE 0x05    //Heart rate waveform (No analysis for now)

#define LOCA_DET_ANOMAL 0x07    //Location detection anomaly
#define OUT_OF_RANGE 0x00       //Detection of targets out of range
#define WITHIN_RANGE 0x01       //Within the scope


class BreathHeart_60GHz{
    private:
        
    public:
        const byte MsgLen = 12;
        byte dataLen = 12;
        byte Msg[12];
        boolean newData = false;
        void SerialInit();
        void recvRadarBytes();
        void Breath_Heart(byte inf[]);
        void Situation_judgment(byte inf[]);
        void ShowData(byte inf[]);
};

#endif
