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

#include "SPI.h"

#define RF69_MAX_DATA_LEN       61 // to take advantage of the built in AES/CRC we want to limit the frame size to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
#define CSMA_LIMIT              -90 // upper RX signal sensitivity threshold in dBm for carrier sense access
#define RF69_MODE_SLEEP         0 // XTAL OFF
#define RF69_MODE_STANDBY       1 // XTAL ON
#define RF69_MODE_SYNTH         2 // PLL ON
#define RF69_MODE_RX            3 // RX MODE
#define RF69_MODE_TX            4 // TX MODE

// available frequency bands
#define RF69_315MHZ            31 // non trivial values to avoid misconfiguration
#define RF69_433MHZ            43
#define RF69_868MHZ            86
#define RF69_915MHZ            91

#define null                  0
#define COURSE_TEMP_COEF    -90 // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69_BROADCAST_ADDR 255
#define RF69_CSMA_LIMIT_MS 1000
#define RF69_TX_LIMIT_MS   1000
#define RF69_FSTEP  61.03515625 // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

// TWS: define CTLbyte bits
#define RFM69_CTL_SENDACK   0x80
#define RFM69_CTL_REQACK    0x40

static volatile unsigned char RFM69_DATA[RF69_MAX_DATA_LEN]; // recv/xmit buf, including header & crc bytes
static volatile unsigned char RFM69_DATALEN;
static volatile unsigned char RFM69_SENDERID;
static volatile unsigned char RFM69_TARGETID; // should match _address
static volatile unsigned char RFM69_PAYLOADLEN;
static volatile unsigned char RFM69_ACK_REQUESTED;
static volatile unsigned char RFM69_ACK_RECEIVED; // should be polled immediately after sending a packet with ACK request
static volatile signed int RFM69_RSSI; // most accurate RSSI during reception (closest to the reception). RSSI of last packet.
static volatile unsigned char RFM69_mode = RF69_MODE_STANDBY;

//RFM69(unsigned char slaveSelectPin, unsigned char interruptPin, unsigned char isRFM69HW, unsigned char interruptNum) //interruptNum is now deprecated
//: RFM69(slaveSelectPin, interruptPin, isRFM69HW) {
//};

//RFM69(unsigned char slaveSelectPin = RF69_SPI_CS, unsigned char interruptPin = RF69_IRQ_PIN, unsigned char isRFM69HW = false);

unsigned char RFM69_initialize(unsigned char ID);
void RFM69_setMode(unsigned char newMode);
void RFM69_setAddress(unsigned char addr);
void RFM69_setNetwork(unsigned char networkID);
unsigned char RFM69_canSend();
unsigned char RFM69_receiveDone();
unsigned char RFM69_ACKReceived(unsigned char fromNodeID);
unsigned char RFM69_ACKRequested();
void RFM69_sendACK(const void* buffer, unsigned char bufferSize);
unsigned long RFM69_getFrequency();
void RFM69_setFrequency(unsigned long freqHz);
void RFM69_encrypt(const char* key);
void RFM69_setCS(unsigned char newSPISlaveSelect);
signed int RFM69_readRSSI(); // *current* signal strength indicator; e.g. < -90dBm says the frequency channel is free + ready to transmit
void RFM69_promiscuous(unsigned char onOff);
void RFM69_setHighPower(unsigned char onOFF); // has to be called after initialize() for RFM69HW
void RFM69_setPowerLevel(unsigned char level); // reduce/increase transmit power level
void RFM69_rcCalibration(); // calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]
void RFM69_receiveBegin();
void RFM69_setHighPowerRegs(unsigned char onOff);
static void isr0();
void interruptHandler();
void interruptHook(unsigned char CTLbyte);
void RFM69_writeReg(unsigned char address, unsigned char data);
unsigned char RFM69_readReg(unsigned char address);

static volatile unsigned char _haveData;

unsigned char _slaveSelectPin;
unsigned char _interruptPin;
unsigned char _interruptNum;
unsigned char _address;
unsigned char _promiscuousMode;
unsigned char _powerLevel;
unsigned char _isRFM69HW;
unsigned char RFM69_promiscuousMode;
#if defined (SPCR) && defined (SPSR)
unsigned char _SPCR;
unsigned char _SPSR;
#endif

#endif