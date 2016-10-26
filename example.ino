//  Library : Arduino HC05 Master Library
//  Author  : Sam33
//  Date    : 2016/10/26

//  Before you use this library, you need to set HC05's baudrate to 9600 and set it's role to Master by yourself.

#include "SmartWatchBT.h"

#define BT_Serial_RX       10
#define BT_Serial_TX       11
#define BT_Key             12

SmartWatch_BT Module_Bluetooth(BT_Key, BT_Serial_RX, BT_Serial_TX);

//  Raw Serial Data from HC05  --  Debug
void BTSerialHandler(char* msg,int8_t c)
{
    Serial.print(msg);
}

//  Scan Timeout
void BTTimeoutHandler()
{
    Serial.print("Scan Timeout"); 
}

//  After you call Scan(), You will recv Dev one by one until you call CancelScan or Timeout
void BTScanResultHandler(SmartWatch_BTDevice Dev)
{
    char buf[30];
    memset(buf,0x0,30);
    sprintf(buf,"%d.%s",BTScanResult+1,Dev.devaddr);
    BTScanResult++;
}

int8_t BTScanResult=0;

void setup() {

    Module_Bluetooth.Init();
    Module_Bluetooth.SetSerialHandler(BTSerialHandler);
    Module_Bluetooth.SetScanResultHandler(BTScanResultHandler);
    Module_Bluetooth.SetTimeoutHandler(BTTimeoutHandler);
	
	/*  Scan Bluetooth Devices */
	/*  
	    BTScanResult=0;
        if(!Module_Bluetooth.Scan())
            Serial.print("Error"); 
    */
	
    /*  Cancel Scaning */
	/*  
        if(!Module_Bluetooth.CancelScan())
            Serial.print("Error"); 
    */
	
    /*  Connect to a Bluetooth Device */
	/*  
	    SmartWatch_BTDevice Dev = XXX ;   // Receive from BTScanResultHandler
        if(!Module_Bluetooth.Connect(Dev))
            Serial.print("Error"); 
    */
	
	/*  Disconnect to a Bluetooth Device */
	/*  
        Module_Bluetooth.Disconnect())
    */
	
	/*  Read & Write
	/*
	    SoftwareSerial x = Module_Bluetooth.serial;  // Read & Write SoftwareSerial by yourself.
	*/
}


void loop() {

}