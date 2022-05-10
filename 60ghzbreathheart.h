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
#define CA_CLOSE 0x01           //Someone approaches
#define CA_AWAY 0x02            //Some people stay away
#define DISORDER 0x03           //Disorderly movement

#define BODY_SIG 0x03           //Body movement information

#define HEART_RATE_RADAR 0x81   //Respiratory heart rate data

#define RATE_DATA 0x01          //Heartbeat Pack
#define RATE_NORMAL 0x01        //Normal heart rate
#define RATE_RAPID 0x02         //Rapid heart rate
#define RATE_SLOW 0x03          //Slow heart rate

#define HEART_RATE 0x02         //Breathing rate

#define HEART_RATE_WAVE 0x03    //Heart rate waveform (No analysis for now)

#define BREATH_DATA 0x04        //Breathing data
#define BREATH_NORMAL 0x01      //Normal breathing
#define BREATH_RAPID 0x02       //Acute respiratory abnormalities
#define BREATH_SLOW 0x03        //Slow heartbeat

#define BREATH_VAL 0x05         //Breathing values

#define BREATH_WAVE 0x06        //Respiratory waveform (No analysis for now)

#define LOCA_DET_ANOMAL 0x07    //Location detection anomaly
#define OUT_OF_RANGE 0x00       //Detection of targets out of range
#define WITHIN_RANGE 0x01       //Within the scope

#define DISTANCE 0x08           //Distance at rest
#define ANGLE 0x09              //Angle at rest


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
