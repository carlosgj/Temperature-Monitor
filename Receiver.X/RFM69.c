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
#include "RFM69.h"
#include "RFM69registers.h"
#include "common.h"
#include "SPI.h"
#include "pindef.h"

//uint8_t RFM69_promiscuousMode = TRUE;
//uint8_t RFM69_powerLevel = 31;

uint8_t RFM69_initialize(uint8_t nodeID, uint8_t networkID) {
    uint8_t CONFIG[][2] = {
        /* 0x01 */
        { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY},
        /* 0x02 */
        { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_01}, // no shaping
        /* 0x03 */
        { REG_BITRATEMSB, 0x00}, // default: 4.8 KBPS
        /* 0x04 */
        { REG_BITRATELSB, 0x80},
        /* 0x05 */
        { REG_FDEVMSB, 0x10}, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
        /* 0x06 */
        { REG_FDEVLSB, 0x00},

        /* 0x07 */
        { REG_FRFMSB, (uint8_t) RF_FRFMSB_915},
        /* 0x08 */
        { REG_FRFMID, (uint8_t) RF_FRFMID_915},
        /* 0x09 */
        { REG_FRFLSB, (uint8_t) RF_FRFLSB_915},

        // looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
        // +17dBm and +20dBm are possible on RFM69HW
        // +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
        // +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
        // +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)
        ///* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_PALEVEL_OUTPUTPOWER_11111},
        ///*0x13 */ 
        { REG_OCP, 0x13 }, // over current protection (default is 95mA)

        // RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
        /* 0x19 */
        { REG_RXBW, RF_RXBW_DCCFREQ_111 | RF_RXBW_MANT_16 | RF_RXBW_EXP_0}, // (BitRate < 2 * RxBw)
        //for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
        
        /* 0x1A */
        { REG_AFCBW, RF_AFCBW_DCCFREQAFC_111 | RF_AFCBW_MANTAFC_16 | RF_AFCBW_EXPAFC_0},
        
        /* 0x25 */
        { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01}, // DIO0 is the only IRQ we're using
        /* 0x26 */
        { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF}, // DIO5 ClkOut disable for power saving
        /* 0x28 */
        { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN}, // writing to this bit ensures that the FIFO & status flags are reset
        /* 0x29 */
        { REG_RSSITHRESH, 220}, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
        ///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
        
        /* 0x2C */
        { REG_PREAMBLEMSB, 0},
        
        /* 0x2D */
        { REG_PREAMBLELSB, 4},
        
        
        /* 0x2E */
        { REG_SYNCCONFIG, RF_SYNC_ON | RF_SYNC_FIFOFILL_AUTO | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0},
        
        /* 0x2F */
        { REG_SYNCVALUE1, 0x2D}, // attempt to make this compatible with sync1 byte of RFM12B lib
        /* 0x30 */
        { REG_SYNCVALUE2, networkID}, // NETWORK ID
        /* 0x37 */
        { REG_PACKETCONFIG1, 0xD0},
        /* 0x38 */
        { REG_PAYLOADLENGTH, 66}, // in variable length mode: the max frame size, not used in TX
        ///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
        /* 0x3C */
        { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE}, // TX on FIFO not empty
        /* 0x3D */
        { REG_PACKETCONFIG2, 3}, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        //for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        /* 0x6F */
        { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0}, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
        {255, 0}
    };

    uint16_t start, now;
    uint8_t timeout = 50;
    uint8_t result = 0;

    RFM69_CS_TRIS = OUTPUT;
    
    RFM69_RST_LAT = FALSE;
    RFM69_RST_TRIS = OUTPUT;
    
    RFM69_reset();
    
    SPI2_Open_RFM69();

    if (result == 0) {
        printf("\tRF: Writing initial config\n");
        RFM69_setMode(RF69_MODE_STANDBY);
        for (uint8_t i = 0; CONFIG[i][0] != 255; i++) {
            RFM69_writeReg(CONFIG[i][0], CONFIG[i][1]);
        }   
    }

    // Encryption is persistent between resets and can trip you up during debugging.
    // Disable it during initialization so we always start from a known state.
    if (result == 0) {
        printf("\tRF: Disabling encryption\n");
        RFM69_encrypt(0);
    }

    if (result == 0) {
        printf("\tRF: Setting power and mode\n");
        RFM69_setHighPower(TRUE); // called regardless if it's a RFM69W or RFM69HW
        RFM69_setMode(RF69_MODE_STANDBY);
    }

    if (result == 0) {
        getMillis(&start);
        while (TRUE) {
            if ((RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) != 0x00) {
                break;
            }
            getMillis(&now);
            if (now > (start + timeout)) {
                printf("\tRF: mode ready timeout!\n");
                result = 1;
                break;
            }
        }
    }

    SPI2_Close();

    printf("\tRF: Enabling interrupt\n");
    //attachInterrupt(_interruptNum, RFM69_isr0, RISING);
    //DIO0 (interrupt) on C7
    RFM69_DIO0_TRIS = INPUT;
    WPUBbits.WPUB2 = TRUE;
    IOCBPbits.IOCBP2 = TRUE;
    PIE0bits.IOCIE = TRUE;

    RF_address = nodeID;
    
    return result;
}

// set the frequency (in Hz)

void RFM69_setFrequency(unsigned long freqHz) {
    uint8_t oldMode = RFM69_mode;
    if (oldMode == RF69_MODE_TX) {
        RFM69_setMode(RF69_MODE_RX);
    }
    freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
    RFM69_writeReg(REG_FRFMSB, (uint8_t) (freqHz >> 16));
    RFM69_writeReg(REG_FRFMID, (uint8_t) (freqHz >> 8));
    RFM69_writeReg(REG_FRFLSB, (uint8_t) freqHz);
    if (oldMode == RF69_MODE_RX) {
        RFM69_setMode(RF69_MODE_SYNTH);
    }
    RFM69_setMode(oldMode);
}

void RFM69_setMode(uint8_t newMode) {
    if (newMode == RFM69_mode)
        return;

    switch (newMode) {
        case RF69_MODE_TX:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
            RFM69_setHighPowerRegs(TRUE);
            break;
        case RF69_MODE_RX:
            RFM69_setHighPowerRegs(FALSE);
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
            break;
        case RF69_MODE_SYNTH:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SYNTHESIZER);
            break;
        case RF69_MODE_STANDBY:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_STANDBY);
            break;
        case RF69_MODE_SLEEP:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_SLEEP);
            break;
        default:
            return;
    }

    // we are using packet mode, so this check is not really needed
    // but waiting for mode ready is necessary when going from sleep because the FIFO may not be immediately available from previous mode
    while (RFM69_mode == RF69_MODE_SLEEP && (RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady

    RFM69_mode = newMode;
}

//set this node's address

void RFM69_setAddress(uint8_t addr) {
    RF_address = addr;
    RFM69_writeReg(REG_NODEADRS, RF_address);
}

// set *transmit/TX* output power: 0=min, 31=max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
// the power configurations are explained in the SX1231H datasheet (Table 10 on p21; RegPaLevel p66): http://www.semtech.com/images/datasheet/sx1231h.pdf
// valid powerLevel parameter values are 0-31 and result in a directly proportional effect on the output/transmission power
// this function implements 2 modes as follows:
//       - for RFM69W the range is from 0-31 [-18dBm to 13dBm] (PA0 only on RFIO pin)
//       - for RFM69HW the range is from 0-31 [5dBm to 20dBm]  (PA1 & PA2 on PA_BOOST pin & high Power PA settings - see section 3.3.7 in datasheet, p22)

void RFM69_setPowerLevel(uint8_t powerLevel) {
    if (RF_powerLevel > 31) {
        RF_powerLevel = 31;
    }
        RF_powerLevel /= 2;
    RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0xE0) | RF_powerLevel);
}

// internal function - interrupt gets called when a packet is received

void RFM69_interruptHandler() {
    //pinMode(4, OUTPUT);
    //digitalWrite(4, 1);
    SPI2_Open_RFM69();
    if (RFM69_mode == RF69_MODE_RX && (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)) {
        RFM69_setMode(RF69_MODE_STANDBY);
        RFM69_CS_LAT = FALSE;
        SPI2Transfer(REG_FIFO & 0x7F);
        
        RFM69_PAYLOADLEN = SPI2Transfer(0);
        //printf("PAYLOADLEN: %02X\n", RFM69_PAYLOADLEN);
        
        if (RFM69_PAYLOADLEN > RF69_MAX_DATA_LEN) {
            RFM69_PAYLOADLEN = RF69_MAX_DATA_LEN; // precaution
        }
        RFM69_TARGETID = SPI2Transfer(0);
        //printf("TARGETID: %02X\n", RFM69_TARGETID);
        if (!(RFM69_TARGETID == RF_address || RFM69_TARGETID == RF69_BROADCAST_ADDR)){ // match this node's address, or broadcast address or anything in promiscuous mode
            RFM69_PAYLOADLEN = 0;
            printf("\tRF: Received packet with incorrect accress.\n");
            RFM69_CS_LAT = TRUE;
            SPI2_Close();
            RFM69_receiveBegin();
            return;
        }
        if(RFM69_PAYLOADLEN < 3){ // address situation could receive packets that are malformed and don't fit this libraries extra fields
            RFM69_PAYLOADLEN = 0;
            printf("\tRF: Received undersize packet.\n");
            RFM69_CS_LAT = TRUE;
            SPI2_Close();
            RFM69_receiveBegin();
            return;
        }

        RFM69_DATALEN = RFM69_PAYLOADLEN - 4;
        RFM69_SENDERID = SPI2Transfer(0);
        //printf("SENDERID: %02X\n", RFM69_SENDERID);
        RFM69_ID = SPI2Transfer(0);
        uint8_t CTLbyte = SPI2Transfer(0);
        //printf("CTLbyte: %02X\n", CTLbyte);

        for (uint8_t i = 0; i < RFM69_DATALEN; i++) {
            RF_data_prelim.rawBytes[i] = SPI2Transfer(0);
        }

        RFM69_CS_LAT = TRUE;
        RFM69_setMode(RF69_MODE_RX);
    }
    RFM69_RSSI = RFM69_readRSSI();
    //printf("RSSI: %d\n", RFM69_RSSI);
    SPI2_Close();
    //printf("Data len: %d\n", RFM69_DATALEN);
    //digitalWrite(4, 0);
}

void RFM69_processPacket(void){
    uint8_t calcChecksum = 0;
    if (RFM69_DATALEN != 11) {
        printf("RF: Got packet with incorrect length\n");
        return;
    }
        
    for (uint8_t i = 0; i < 10; i++) {
        calcChecksum ^= RF_data_prelim.rawBytes[i];
    }
    
    if(calcChecksum != RF_data_prelim.checksum){
        printf("RF: Got packet with bad checksum\n");
        return;
    }
    
    memcpy(&RF_data_good, &RF_data_prelim, 11);
    newRFDataFlag = TRUE;
}

void RFM69_send(const uint8_t* data, uint8_t len){
    uint8_t i, alreadyOpen;
    
    alreadyOpen = SPI2CON0bits.EN;
    
    if(!alreadyOpen){
        SPI2_Open_RFM69();
    }
    
    RFM69_setMode(RF69_MODE_STANDBY); // Prevent RX while filling the fifo
    
    printf("Sending %d bytes: header+", len + 4);
    for(i=0; i<len; i++){
        printf(" %02X", data[i]);
    }
    printf("\n");

    RFM69_CS_LAT = FALSE;
    SPI2Transfer(REG_FIFO & 0x7F);
    SPI2Transfer(len + 4); // Include length of headers
    // First the 4 headers
    SPI2Transfer(0x00);
    SPI2Transfer(0x00);
    SPI2Transfer(0x00);
    SPI2Transfer(0x00);
    // Now the payload
    while (len--){
        SPI2Transfer(*data++);
    }
    RFM69_CS_LAT = TRUE;

    RFM69_setMode(RF69_MODE_TX); // Start the transmitter
    
    if(!alreadyOpen){
        SPI2_Close();
    }
    __delay_ms(10);
}


void RFM69_receiveBegin() {
    //printf("RF: Receive Begin\n");
    RFM69_DATALEN = 0;
    RFM69_SENDERID = 0;
    RFM69_TARGETID = 0;
    RFM69_PAYLOADLEN = 0;
    RFM69_RSSI = 0;
    SPI2_Open_RFM69();
    if (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY){
        RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
    }
    RFM69_writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
    RFM69_setMode(RF69_MODE_RX);
    SPI2_Close();
}

// checks if a packet was received and/or puts transceiver in receive (ie RX or listen) mode

uint8_t RFM69_receiveDone() {
    //printf("ReceiveDone\n");
    if(RF_haveData) {
        //printf("RF: Have data!\n");
        RF_haveData = FALSE;
        RFM69_interruptHandler();
    }
    if (RFM69_mode == RF69_MODE_RX && RFM69_PAYLOADLEN > 0) {
        SPI2_Open_RFM69();
        RFM69_setMode(RF69_MODE_STANDBY); // enables interrupts
        SPI2_Close();
        return TRUE;
    }
    else{
        if (RFM69_mode == RF69_MODE_RX){ // already in RX no payload yet
            return FALSE;
        }
    }
    RFM69_receiveBegin();
    return FALSE;
}

// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!

void RFM69_encrypt(const char* key) {
    RFM69_setMode(RF69_MODE_STANDBY);
    if (key != 0) {
        RFM69_CS_LAT = FALSE;
        SPI2Transfer(REG_AESKEY1 | 0x80);
        for (uint8_t i = 0; i < 16; i++)
            SPI2Transfer(key[i]);
        RFM69_CS_LAT = TRUE;
    }
    RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFE) | (key ? 1 : 0));
}

// get the received signal strength indicator (RSSI)

signed int RFM69_readRSSI() {
    uint8_t forceTrigger = FALSE;
    signed int rssi = 0;
    if (forceTrigger) {
        // RSSI trigger not needed if DAGC is in continuous mode
        RFM69_writeReg(REG_RSSICONFIG, RF_RSSI_START);
        while ((RFM69_readReg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00); // wait for RSSI_Ready
    }
    rssi = -RFM69_readReg(REG_RSSIVALUE);
    rssi >>= 1;
    return rssi;
}

// for RFM69HW only: you must call setHighPower(TRUE) after initialize() or else transmission won't work

void RFM69_setHighPower(uint8_t onOff) {
    //RF_isRFM69HW = onOff;
    RFM69_writeReg(REG_OCP, RF_OCP_OFF);
        RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
}

// internal function

void RFM69_setHighPowerRegs(uint8_t onOff) {
    RFM69_writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
    RFM69_writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

void RFM69_rcCalibration() {
    RFM69_writeReg(REG_OSC1, RF_OSC1_RCCAL_START);
    while ((RFM69_readReg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00);
}

void RFM69_writeReg(uint8_t address, uint8_t data) {
    address |= 0b10000000; //Set MSB to 1 for write
    RFM69_CS_LAT = FALSE; //select radio
    SPI2Transfer(address);
    SPI2Transfer(data);
#ifdef DEBUG_RF
    printf("\tRF: Wrote %02X to %02X\n", data, address&0b01111111);
#endif
    RFM69_CS_LAT = TRUE; //Deselect radio
}

uint8_t RFM69_readReg(uint8_t address) {
    address &= 0b01111111; //Set MSB to 0 for read
    RFM69_CS_LAT = FALSE; //select radio
    SPI2Transfer(address);
    uint8_t result = SPI2Transfer(0);
    RFM69_CS_LAT = TRUE; //Deselect radio
#ifdef DEBUG_RF
    printf("\tRF: Read %02X from %02X\n", result, address&0b01111111);
#endif
    return result;
}

void RFM69_reset(void){
    RFM69_RST_LAT = TRUE;
    __delay_ms(1);
    RFM69_RST_LAT = FALSE;
    __delay_ms(10);
}