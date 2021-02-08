/*  Title:  Custom minimalistic Sensirion SFM3019 Library for basic functionality
 *  
 *  Version:  v0.1
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

#include "SFM3019_HSKA.h"                           //when placed in same folder
//#include <SFM3019_HSKA.h>                         //when placed in standard library folder
#include <Wire.h>

//  some values needed for calculation of physical flow and temperature values
#define SFM3019Offset 24576
#define SFM3019ScaleFactorFlow 170.0                //needs to be a float, otherwise it will not calculate in float
#define SFM3019ScaleFactorTemp 200.0                //needs to be a float, otherwise it will not calculate in float


SFM3019_HSKA::SFM3019_HSKA(uint8_t i2cAddress)      //constructor
{
  SFM3019addr = i2cAddress;                         //safe I2C address to generated class object
}


/*  checks if a Device at the desired address is responding with an ACK */
int8_t SFM3019_HSKA::i2c_device_check(){
  Wire.beginTransmission(SFM3019addr);              //begin I2C transmission Address (i)
  if (Wire.endTransmission() == 0)                  //receive 0 = success (ACK response) 
    {
      Serial.print ("Found Seosor at address");
      Serial.print (SFM3019addr, DEC);
      Serial.print (" (0x");
      Serial.print (SFM3019addr, HEX);              // 7 bit address
      Serial.println (")");
      return 0;                                     //0=device sent ACK
      
    }else{
      
      Serial.print ("Did not receive Acknowledge from I2C address ");
      Serial.print (SFM3019addr, DEC);
      Serial.print (" (0x");
      Serial.print (SFM3019addr, HEX);              // 7 bit address
      Serial.println (")");
      return 1;                                     //no ACK received
    }
}



/*  writes a 16bit "SendData" to I2C Bus Device "address" */
int8_t SFM3019_HSKA::i2c_write(const uint16_t SendData) {
    Wire.beginTransmission(SFM3019addr);
    //fill I2C outbuffer
    Wire.write((SendData>>8)& 0xFF);                //isolate HighByte
    Wire.write(SendData & 0xFF);                    //isolate LowByte
    Wire.endTransmission();                         //send I2C outbuffer and end transmission (Stop)

    return 0;                                       
}



/*  reads all 9 measurement bytes for flow, temp and status */
SFM3019_HSKA::SensVal_s SFM3019_HSKA::i2c_read_all(){
    SensVal_s SensVal = {0};                        //create empty struct
    Wire.requestFrom(SFM3019addr, 9, true);         //request 9byte (3x16bit + CRC) from the sensor
   //while(Wire.available()<3){};                   //wait for all the data to be received //ATTENTION may be critical loop forever, however not using this may lead to an error, as the Buffer may be processed faster, then the input is read on I2C
    
    //get Flow Bytes
    int16_t flow = Wire.read()<<8;                  //get Highbyte and shift 8bit to 8MSB
    flow = flow | Wire.read();                      //get Lowbyte 8LSB
    byte CRCflow = Wire.read();                     //get CRC Check Byte (you could do a data validy check with that)    
    //Serial.println(flow, HEX);                    //raw values for debugging
    SensVal.flow = (flow + SFM3019Offset) / SFM3019ScaleFactorFlow;     //calculate the flow in slm as Datasheet mentions        

    //get Temperature Bytes
    int16_t temp = Wire.read()<<8;                  //get Highbyte and shift 8bit to 8MSB
    temp = temp | Wire.read();                      //get Lowbyte 8LSB
    byte CRCtemp = Wire.read();                     //get CRC Check Byte (you could do a data validy check with that)    
    //Serial.println(temp, HEX);                    //raw values for debugging
    SensVal.temp = temp / SFM3019ScaleFactorTemp;   //calculate the flow in slm as Datasheet mentions        

    //get StatusWord Bytes
    int16_t stat = Wire.read()<<8;                  //get Highbyte and shift 8bit to 8MSB
    stat = stat | Wire.read();                      //get Lowbyte 8LSB
    byte CRCstat = Wire.read();                     //get CRC Check Byte (you could do a data validy check with that)    
    //Serial.println(stat, HEX);                    //raw values for debugging
    SensVal.statusword = temp / SFM3019ScaleFactorTemp;               //calculate the flow in slm as Datasheet mentions        

    //return all data as struct
    return SensVal;
    
}



/*  reads only first 3 bytes for flow and does NO CRC!*/
float SFM3019_HSKA::i2c_read_flow(){
    Wire.requestFrom(SFM3019addr, 3, true);         // Request 9byte (2x16bit + CRC) from the sensor
    //while(Wire.available()<3){};                  //wait for all the data to be received //ATTENTION may be critical loop forever, however not using this may lead to an error, as the Buffer may be processed faster, then the input is read on I2C
    int16_t flow = Wire.read()<<8;                  //get Highbyte and shift 8bit to 8MSB
    flow = flow | Wire.read();                      //get Lowbyte 8LSB
    byte CRC = Wire.read();                         //get CRC Check Byte (you could do a data validy check with that)    
    //Serial.println(flow, HEX);                    //raw values for debugging
    return (flow + SFM3019Offset) / SFM3019ScaleFactorFlow;           //calculate the flow in slm as Datasheet mentions  
}

/*  reads only first 3 bytes for flow and DOES CRC*/
float SFM3019_HSKA::i2c_read_flow_CRC(){
    Wire.requestFrom(SFM3019addr, 3, true);         // Request 9byte (2x16bit + CRC) from the sensor
    //while(Wire.available()<3){};                  //wait for all the data to be received //ATTENTION may be critical loop forever, however not using this may lead to an error, as the Buffer may be processed faster, then the input is read on I2C
    uint8_t Highbyte = Wire.read();                 //get Highbyte 8MSB
    uint8_t Lowbyte =  Wire.read();                 //get Lowbyte 8LSB
    uint8_t CRC = Wire.read();                      //get CRC Check Byte

    //Confirm CRC
    uint8_t mycrc = 0xFF;                           //initialize crc variable
    mycrc = crc8(Highbyte, mycrc);                  //let first byte through CRC calculation
    mycrc = crc8(Lowbyte, mycrc);                   //and the second byte too
    if (mycrc != CRC) {                             //check if the calculated and the received CRC byte matches
      //Serial.println("Error: wrong CRC");
      return -10000;                                //extreme low value, so user knows somethig is wrong
    } else {
      //Serial.println("Success: identical CRC");
      int16_t flow = (Highbyte<<8) | Lowbyte;       //stack the to bytes together as signed int16
      return (flow + SFM3019Offset) / SFM3019ScaleFactorFlow;         //calculate the flow in slm as Datasheet mentions  
    }
}

/*  calculate a CRC Byte, (Cyclic Redundancy Check) */
uint8_t SFM3019_HSKA::crc8(const uint8_t data, uint8_t crc)
{
     crc ^= data;                                 //crc XOR data
     for ( uint8_t i = 8; i; --i ) {              //search on the internet for crc8 algorithm for more information, its a standard
       crc = ( crc & 0x80 )           
       ? (crc << 1) ^ 0x31
       : (crc << 1);
    }
  return crc;
}
