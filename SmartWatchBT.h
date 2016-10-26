#ifndef _SmartWatch_BT_H_
#define _SmartWatch_BT_H
#include "SoftwareSerial.h"
#include <Arduino.h>
/* HC05 */
class SmartWatch_BTDevice
{
  public:
  char devname[12];  
  char devaddr[18];
};


#define BTState_Disable   0x00
#define BTState_Normal    0x01
#define BTState_Execute   0x02
#define SerialTimeout     10000     //second


#define Action_None        0x00
#define Action_Scan        0x01
#define Action_Test        0x02
#define Action_CancelScan  0x03
#define Action_Connect     0x04

#define ErrorCode_Correct  0x00
#define ErrorCode_Timeout  0x01
#define ErrorCode_Overflow 0x02

#define MAX_RECVBUF        60


class SmartWatch_BT
{
  
  private:
  int8_t ClearSerial(){
    int8_t c=0;
    while (serial.available()){
      serial.read();
      c++;  
    }
  }
  
  
bool tryParseDevice(char *str,SmartWatch_BTDevice &device)
{
  //Serial.print("Parse:");
  //Serial.println(str);
  if(strlen(str)<18)
    return false;
  //INQ:2014:4:242613,1F00,FFB2
  char * tail = strstr(str,",");
  if(tail==NULL)
    return false;
  int8_t tailindex = tail-str;
  int8_t headindex = 4;
  if(tailindex<headindex || tailindex-headindex>18)
    return false;
  memset(device.devaddr,0x0,18);
  memcpy(device.devaddr,str+headindex,tailindex-headindex);
  for(int i=0 ; i<strlen(device.devaddr) ; i++){
    if(device.devaddr[i]==':')
      device.devaddr[i]=',';
  }
  return true;
}

  public:
  SoftwareSerial serial;
  int8_t state;
  int8_t cmdpin;
  int8_t action;
  char rcvbuf[MAX_RECVBUF];
  int8_t rcvbufindex;
  char charbuf;
  unsigned long int timestamp;
  void (*SerialHandler)(char*,int8_t) = NULL;
  void (*ScanResultHandler)(SmartWatch_BTDevice) = NULL;
  void (*TimeOutHandler)(void) = NULL;
  int8_t errcode;
  void SetTimeoutHandler(void (*func)(void))
  {
    TimeOutHandler = func;  
  }
  
  void SetSerialHandler(void (*func)(char*,int8_t))
  {
    SerialHandler = func;
  }
  
  void SetScanResultHandler(void (*func)(SmartWatch_BTDevice))
  {
    ScanResultHandler = func;
  }
  
  SmartWatch_BT(uint8_t CMD , uint8_t RX , uint8_t TX) : serial(RX,TX)
  {
    cmdpin = CMD;
  }
  
  void Init()
  {
    pinMode(cmdpin,OUTPUT);
    digitalWrite(cmdpin,HIGH);
    serial.begin(9600);  
    state = BTState_Normal;
    action = Action_None;
    errcode = ErrorCode_Correct;
  }
  void Update()
  {
    if(state == BTState_Execute){
      switch(action){
         case Action_Scan:{
           memset(rcvbuf,0x0,MAX_RECVBUF);
           rcvbufindex=0;
           while (serial.available()){
            char c = serial.read();
            if(c=='K'){
              if(rcvbuf[rcvbufindex-1]=='O'){
                ClearSerial();
                state=BTState_Normal;
                action=Action_None;
                Serial.println(F("Action_Scan-->OK"));
              }
            }else if(c=='+'){
              SmartWatch_BTDevice dev;
              String devstr(rcvbuf);
              if(tryParseDevice(rcvbuf,dev)){
                if(ScanResultHandler!=NULL){
                  ScanResultHandler(dev);
                }
              }
              memset(rcvbuf,0x0,MAX_RECVBUF);
              rcvbufindex=0;
            }else{
              if(rcvbufindex<MAX_RECVBUF){
                rcvbuf[rcvbufindex++]=c;
              }else{
                ClearSerial();
                state=BTState_Normal;
                action = Action_None;
                errcode = ErrorCode_Overflow;
                Serial.println(F("######Overflow")); 
              }
            }
           }
         } 
         break;
         default:
         break;
      }
      if(millis()-timestamp>SerialTimeout || millis()<timestamp)
      {
        int8_t c = ClearSerial();
        state=BTState_Normal;
        action = Action_None;
        errcode = ErrorCode_Timeout;
        Serial.println(F("#Timeout"));
        if(TimeOutHandler!=NULL)
          TimeOutHandler();
      }
    }
  }
  
  void Disconnect()
  {
    digitalWrite(cmdpin,LOW);
    delay(100);
    digitalWrite(cmdpin,HIGH);
    Serial.println(F("AT+DISC Excute"));
    serial.print(F("AT+DISC\r\t"));
  }
  
  bool Connect(SmartWatch_BTDevice Dev)
  {
    if(state==BTState_Normal)
    {
      int8_t c = ClearSerial();
      ClearSerial();
      timestamp = millis();
      state = BTState_Execute;
      action = Action_Connect;
      memset(rcvbuf,0x0,MAX_RECVBUF);  //Memory to few .... I use rcvbuf for building string
      sprintf(rcvbuf,"AT+LINK=%s\r\n",Dev.devaddr);
      Serial.println(F("AT+LINK Excute"));
      serial.print(rcvbuf);
      return true;
    }
    return false;
  }
  
  bool Scan()
  {
     if(state==BTState_Normal)
     {
       int8_t c = ClearSerial();
       ClearSerial();
       timestamp = millis();
       state = BTState_Execute;
       action = Action_Scan;
       //deviceamount=0;
       Serial.println(F("AT+INQ Excute"));
       serial.print(F("AT+INQ\r\t"));
       return true;
     }
     return false;
  } 
  
  bool CancelScan()
  {
     if(state==BTState_Execute && action == Action_Scan)
     {
       int8_t c = ClearSerial();
       ClearSerial();
       timestamp = millis();
       state=BTState_Execute;
       action = Action_CancelScan;
       Serial.println(F("AT+INQC Excute"));
       serial.print(F("AT+INQC\r\t"));
       return true;
     }    
     return false;
  }
  
  void SendChar()
  {
    serial.write(charbuf);  
  }
  
  void Test()
  {
     if(state==BTState_Normal)
     {
       int8_t c = ClearSerial();
       Serial.print("Clear:");
       Serial.println(c);
       timestamp = millis();
       state = BTState_Execute;
       action = Action_Test;
       Serial.println("AT Excute");
       serial.print("AT\r\t");
     }
  } 
  
  
};

#endif