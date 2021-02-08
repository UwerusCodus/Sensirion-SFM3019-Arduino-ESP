/*  Title:  Custom minimalistic Sensirion SFM3019 Library for basic functionality
 *  
 *  Version:  v1.1
 *  Autor:  Marcel Franz
 *  Date: 18.Sep.2020
 *  
 *  Short Description:
 *  due to the lack of a minimalistic library, I decided to write an own, corresponding to the registers
 *  given in the Datasheet. 
 *  
 *  Changelog:
 *  17.09.2020 just test basic function in skript, no separated .h and .cpp file
 *  18.09.2020 Separation in header and C++ File
 *  18.09.2020 added function to get the flow value, checked with CRC
*/

#include "SFM3019_HSKA.h"
#include <Wire.h>

SFM3019_HSKA SFM3019(0x2E);                       //Generate Object SFM3019 of class SFM3019_HSKA
SFM3019_HSKA::SensVal_s SensVal = {0, 0, 0};      //Generate variable (struct) SensVal{flow,temp,status}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  Wire.begin (21, 22);   // for ESP32: SDA= GPIO_21 /SCL= GPIO_22

  SFM3019.i2c_device_check();                     //checks if SFM3019 is available
  SFM3019.i2c_write(0x3608);                      //write 2Byte 0x3608 to start continuous measurement
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:

  //get only flow value, NO!! CRC confirmation
  SensVal.flow = SFM3019.i2c_read_flow();     //read only Flow Bytes
  Serial.print("Flow: ");
  Serial.println(SensVal.flow, 27);
  delay(100);
  
  
  //get only flow value witch CRC confirmation
  SensVal.flow = SFM3019.i2c_read_flow_CRC();     //read only Flow Bytes
  Serial.print("CRC checked flow: ");
  Serial.println(SensVal.flow, 27);
  delay(100);

  
  //get complete measurement data: flow, temperature and statusword
  SensVal = SFM3019.i2c_read_all();               //read all Sensor Bytes
  Serial.print("Flow: ");
  if (SensVal.flow >= 0) Serial.print(" ");       //just for beauty reasons
  Serial.print(SensVal.flow,10);
  Serial.print("  Temp: ");
  Serial.print(SensVal.temp,3);
  Serial.print("  Statusword: 0x");
  Serial.print(SensVal.statusword, HEX);
  Serial.println();
  
  delay(1000);
}
