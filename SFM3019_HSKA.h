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

#ifndef SFM3019_HSKA_H                                      // include guard
#define SFM3019_HSKA_H  

#include "Arduino.h"



    class SFM3019_HSKA {
      
    public: //can be accessed public

        SFM3019_HSKA(uint8_t i2cAddress);                   //Constructor
    
        typedef struct {float flow; float temp; int16_t statusword;} SensVal_s;     //Struct for complete Sensor Readout
        SensVal_s SensVal = {0, 0, 0};                      //initialize with 0

        int8_t i2c_device_check();                          //function to check if SFM3019 is available on I2C
        int8_t i2c_write(const uint16_t SendData);          //function to write 16Bit Data to sensor
        SensVal_s i2c_read_all();                           //function to read Flow, Temperature and Statusword
        float i2c_read_flow();                              //function to only read Flow
        float i2c_read_flow_CRC();                          //function to only read Flow with CRC check



    private:  //can only be accessed by methods of same object
        
        uint8_t SFM3019addr = 46;                           //SFM3019 I2C Adress: 46 / 0x2E
        uint8_t crc8(const uint8_t data, uint8_t crc);      //fuction for CRC confirmation
    };

#endif
