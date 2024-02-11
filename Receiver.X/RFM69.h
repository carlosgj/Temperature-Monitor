// **********************************************************************************
// Driver definition for HopeRF RFM69W/RFM69HW/RFM69CW/RFM69HCW, Semtech SX1231/1231H
// **********************************************************************************
// Copyright LowPowerLab LLC 2018, https://www.LowPowerLab.com/contact
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#ifndef RFM69_H
#define RFM69_H

#include "common.h"
#include "SPI.h"
#include <string.h>

//#define DEBUG_RF

#define RF69_MAX_DATA_LEN       61 // to take advantage of the built in AES/CRC we want to limit the frame size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
#define CSMA_LIMIT              -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP         0 // XTAL OFF
#define RF69_MODE_STANDBY       1 // XTAL ON
#define RF69_MODE_SYNTH         2 // PLL ON
#define RF69_MODE_RX            3 // RX MODE
#define RF69_MODE_TX            4 // TX MODE

// available frequency bands
#define RF69_915MHZ            91

#define COURSE_TEMP_COEF    -90 // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69_BROADCAST_ADDR 255
#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS   1000
#define RF69_FSTEP  61.03515625 // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

union RF_Data_t {
    uint8_t rawBytes[RF69_MAX_DATA_LEN];

    struct {
        uint16_t packetID;
        uint16_t temperature;
        uint16_t batteryVoltage;
        uint16_t solarVoltage;
        uint16_t internalTemp;
        uint8_t checksum;
    };
} RF_data_prelim, RF_data_good;

volatile uint8_t RFM69_DATALEN;
volatile uint8_t RFM69_SENDERID;
volatile uint8_t RFM69_TARGETID; // should match _address
volatile uint8_t RFM69_ID;
volatile uint8_t RFM69_PAYLOADLEN;
volatile signed int RFM69_RSSI; // most accurate RSSI during reception (closest to the reception). RSSI of last packet.
volatile uint8_t RFM69_mode = RF69_MODE_STANDBY;

uint8_t RFM69_initialize(uint8_t ID, uint8_t networkID);
void RFM69_setMode(uint8_t newMode);
void RFM69_setAddress(uint8_t addr);
uint8_t RFM69_receiveDone(void);
void RFM69_sendACK(const void* buffer, uint8_t bufferSize);
void RFM69_setFrequency(unsigned long freqHz);
void RFM69_encrypt(const char* key);
signed int RFM69_readRSSI(void); // *current* signal strength indicator; e.g. < -90dBm says the frequency channel is free + ready to transmit
void RFM69_setHighPower(uint8_t onOFF); // has to be called after initialize() for RFM69HW
void RFM69_setPowerLevel(uint8_t level); // reduce/increase transmit power level
void RFM69_rcCalibration(); // calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]
void RFM69_receiveBegin(void);
void RFM69_setHighPowerRegs(uint8_t onOff);
//static void isr0();
void RFM69_interruptHandler();
void RFM69_writeReg(uint8_t address, uint8_t data);
uint8_t RFM69_readReg(uint8_t address);
void RFM69_send(const uint8_t* data, uint8_t len);
void RFM69_processPacket(void);
void RFM69_reset(void);

volatile uint8_t RF_haveData;

uint8_t RF_address;
uint8_t RF_powerLevel;

#endif