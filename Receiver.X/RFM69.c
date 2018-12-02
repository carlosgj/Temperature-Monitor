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

volatile unsigned char RFM69_haveData;

unsigned char RFM69_promiscuousMode = TRUE;
unsigned char RFM69_powerLevel = 31;

unsigned char RFM69_initialize(unsigned char nodeID) {
    unsigned char CONFIG[][2] ={
        /* 0x01 */
        { REG_OPMODE, RF_OPMODE_SEQUENCER_ON | RF_OPMODE_LISTEN_OFF | RF_OPMODE_STANDBY},
        /* 0x02 */
        { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_FSK | RF_DATAMODUL_MODULATIONSHAPING_00}, // no shaping
        /* 0x03 */
        { REG_BITRATEMSB, RF_BITRATEMSB_1200}, // default: 4.8 KBPS
        /* 0x04 */
        { REG_BITRATELSB, RF_BITRATELSB_1200},
        /* 0x05 */
        { REG_FDEVMSB, RF_FDEVMSB_5000}, // default: 5KHz, (FDEV + BitRate / 2 <= 500KHz)
        /* 0x06 */
        { REG_FDEVLSB, RF_FDEVLSB_5000},

        /* 0x07 */
        { REG_FRFMSB, (unsigned char) RF_FRFMSB_915},
        /* 0x08 */
        { REG_FRFMID, (unsigned char) RF_FRFMID_915},
        /* 0x09 */
        { REG_FRFLSB, (unsigned char) RF_FRFLSB_915},

        // looks like PA1 and PA2 are not implemented on RFM69W, hence the max output power is 13dBm
        // +17dBm and +20dBm are possible on RFM69HW
        // +13dBm formula: Pout = -18 + OutputPower (with PA0 or PA1**)
        // +17dBm formula: Pout = -14 + OutputPower (with PA1 and PA2)**
        // +20dBm formula: Pout = -11 + OutputPower (with PA1 and PA2)** and high power PA settings (section 3.3.7 in datasheet)
        ///* 0x11 */ { REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_PALEVEL_OUTPUTPOWER_11111},
        ///* 0x13 */ { REG_OCP, RF_OCP_ON | RF_OCP_TRIM_95 }, // over current protection (default is 95mA)

        // RXBW defaults are { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_5} (RxBw: 10.4KHz)
        /* 0x19 */
        { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_16 | RF_RXBW_EXP_2}, // (BitRate < 2 * RxBw)
        //for BR-19200: /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 | RF_RXBW_MANT_24 | RF_RXBW_EXP_3 },
        /* 0x25 */
        { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01}, // DIO0 is the only IRQ we're using
        /* 0x26 */
        { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF}, // DIO5 ClkOut disable for power saving
        /* 0x28 */
        { REG_IRQFLAGS2, RF_IRQFLAGS2_FIFOOVERRUN}, // writing to this bit ensures that the FIFO & status flags are reset
        /* 0x29 */
        { REG_RSSITHRESH, 220}, // must be set to dBm = (-Sensitivity / 2), default is 0xE4 = 228 so -114dBm
        ///* 0x2D */ { REG_PREAMBLELSB, RF_PREAMBLESIZE_LSB_VALUE } // default 3 preamble bytes 0xAAAAAA
        /* 0x2E */
        { REG_SYNCCONFIG, RF_SYNC_OFF | RF_SYNC_FIFOFILL_MANUAL | RF_SYNC_SIZE_2 | RF_SYNC_TOL_0},
        /* 0x2F */
        { REG_SYNCVALUE1, 0x2D}, // attempt to make this compatible with sync1 byte of RFM12B lib
        /* 0x30 */
        { REG_SYNCVALUE2, 123}, // NETWORK ID
        /* 0x37 */
        { REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF | RF_PACKET1_CRCAUTOCLEAR_ON | RF_PACKET1_ADRSFILTERING_OFF},
        /* 0x38 */
        { REG_PAYLOADLENGTH, 66}, // in variable length mode: the max frame size, not used in TX
        ///* 0x39 */ { REG_NODEADRS, nodeID }, // turned off because we're not using address filtering
        /* 0x3C */
        { REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RF_FIFOTHRESH_VALUE}, // TX on FIFO not empty
        /* 0x3D */
        { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_2BITS | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF}, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        //for BR-19200: /* 0x3D */ { REG_PACKETCONFIG2, RF_PACKET2_RXRESTARTDELAY_NONE | RF_PACKET2_AUTORXRESTART_ON | RF_PACKET2_AES_OFF }, // RXRESTARTDELAY must match transmitter PA ramp-down time (bitrate dependent)
        /* 0x6F */
        { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA0}, // run DAGC continuously in RX mode for Fading Margin Improvement, recommended default for AfcLowBetaOn=0
        {255, 0}
    };

    unsigned long start = ms_count;
    unsigned char timeout = 50;
    do RFM69_writeReg(REG_SYNCVALUE1, 0xAA); while (RFM69_readReg(REG_SYNCVALUE1) != 0xaa && ms_count - start < timeout);
    start = ms_count;
    do RFM69_writeReg(REG_SYNCVALUE1, 0x55); while (RFM69_readReg(REG_SYNCVALUE1) != 0x55 && ms_count - start < timeout);

    for (unsigned char i = 0; CONFIG[i][0] != 255; i++)
        RFM69_writeReg(CONFIG[i][0], CONFIG[i][1]);

    // Encryption is persistent between resets and can trip you up during debugging.
    // Disable it during initialization so we always start from a known state.
    RFM69_encrypt(0);

    RFM69_setHighPower(_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
    RFM69_setMode(RF69_MODE_STANDBY);
    start = ms_count;
    while (((RFM69_readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00) && ms_count - start < timeout); // wait for ModeReady
    if (ms_count - start >= timeout)
        return FALSE;
    //attachInterrupt(_interruptNum, RFM69_isr0, RISING);

    _address = nodeID;
    return TRUE;
}

// return the frequency (in Hz)

unsigned long RFM69_getFrequency() {
    return (unsigned long) (RF69_FSTEP * (((unsigned long) RFM69_readReg(REG_FRFMSB) << 16) + ((unsigned int) RFM69_readReg(REG_FRFMID) << 8) + RFM69_readReg(REG_FRFLSB)));
}

// set the frequency (in Hz)

void RFM69_setFrequency(unsigned long freqHz) {
    unsigned char oldMode = RFM69_mode;
    if (oldMode == RF69_MODE_TX) {
        RFM69_setMode(RF69_MODE_RX);
    }
    freqHz /= RF69_FSTEP; // divide down by FSTEP to get FRF
    RFM69_writeReg(REG_FRFMSB, freqHz >> 16);
    RFM69_writeReg(REG_FRFMID, freqHz >> 8);
    RFM69_writeReg(REG_FRFLSB, freqHz);
    if (oldMode == RF69_MODE_RX) {
        RFM69_setMode(RF69_MODE_SYNTH);
    }
    RFM69_setMode(oldMode);
}

void RFM69_setMode(unsigned char newMode) {
    if (newMode == RFM69_mode)
        return;

    switch (newMode) {
        case RF69_MODE_TX:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_TRANSMITTER);
            if (_isRFM69HW) RFM69_setHighPowerRegs(TRUE);
            break;
        case RF69_MODE_RX:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
            if (_isRFM69HW) RFM69_setHighPowerRegs(FALSE);
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

//put transceiver in sleep mode to save battery - to wake or resume receiving just call receiveDone()

void RFM69_sleep() {
    RFM69_setMode(RF69_MODE_SLEEP);
}

//set this node's address

void RFM69_setAddress(unsigned char addr) {
    _address = addr;
    RFM69_writeReg(REG_NODEADRS, _address);
}

//set this node's network id

void RFM69_setNetwork(unsigned char networkID) {
    RFM69_writeReg(REG_SYNCVALUE2, networkID);
}

// set *transmit/TX* output power: 0=min, 31=max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
// the power configurations are explained in the SX1231H datasheet (Table 10 on p21; RegPaLevel p66): http://www.semtech.com/images/datasheet/sx1231h.pdf
// valid powerLevel parameter values are 0-31 and result in a directly proportional effect on the output/transmission power
// this function implements 2 modes as follows:
//       - for RFM69W the range is from 0-31 [-18dBm to 13dBm] (PA0 only on RFIO pin)
//       - for RFM69HW the range is from 0-31 [5dBm to 20dBm]  (PA1 & PA2 on PA_BOOST pin & high Power PA settings - see section 3.3.7 in datasheet, p22)

void RFM69_setPowerLevel(unsigned char powerLevel) {
    if (_powerLevel > 31) {
        _powerLevel = 31;
    }
    if (_isRFM69HW) {
        _powerLevel /= 2;
    }
    RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0xE0) | _powerLevel);
}

unsigned char RFM69_canSend() {
    if (RFM69_mode == RF69_MODE_RX && RFM69_PAYLOADLEN == 0 && RFM69_readRSSI() < CSMA_LIMIT) // if signal stronger than -100dBm is detected assume channel activity
    {
        RFM69_setMode(RF69_MODE_STANDBY);
        return TRUE;
    }
    return FALSE;
}

// internal function - interrupt gets called when a packet is received

void RFM69_interruptHandler() {
    //pinMode(4, OUTPUT);
    //digitalWrite(4, 1);
    if (RFM69_mode == RF69_MODE_RX && (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)) {
        RFM69_setMode(RF69_MODE_STANDBY);
        RFM69_CS_LAT = FALSE;
        SPI2Transfer(REG_FIFO & 0x7F);
        RFM69_PAYLOADLEN = 20;
        if (RFM69_PAYLOADLEN > 66) {
            RFM69_PAYLOADLEN = 66; // precaution
        }
        RFM69_TARGETID = SPI2Transfer(0);
        if (!(RFM69_promiscuousMode || RFM69_TARGETID == _address || RFM69_TARGETID == RF69_BROADCAST_ADDR) // match this node's address, or broadcast address or anything in promiscuous mode
                || RFM69_PAYLOADLEN < 3) // address situation could receive packets that are malformed and don't fit this libraries extra fields
        {
            RFM69_PAYLOADLEN = 0;
            RFM69_CS_LAT = TRUE;
            RFM69_receiveBegin();
            return;
        }

        RFM69_DATALEN = RFM69_PAYLOADLEN - 3;
        RFM69_SENDERID = SPI2Transfer(0);
        unsigned char CTLbyte = SPI2Transfer(0);

        RFM69_ACK_RECEIVED = CTLbyte & RFM69_CTL_SENDACK; // extract ACK-received flag
        RFM69_ACK_REQUESTED = CTLbyte & RFM69_CTL_REQACK; // extract ACK-requested flag

        //interruptHook(CTLbyte); // TWS: hook to derived class interrupt function

        for (unsigned char i = 0; i < RFM69_DATALEN; i++) {
            RFM69_DATA[i] = SPI2Transfer(0);
        }
        if (RFM69_DATALEN < RF69_MAX_DATA_LEN) RFM69_DATA[RFM69_DATALEN] = 0; // add null at end of string
        RFM69_CS_LAT = TRUE;
        RFM69_setMode(RF69_MODE_RX);
    }
    RFM69_RSSI = RFM69_readRSSI();
    //digitalWrite(4, 0);
}

// internal function

void RFM69_isr0() {
    _haveData = TRUE;
}

// internal function

void RFM69_receiveBegin() {
    RFM69_DATALEN = 0;
    RFM69_SENDERID = 0;
    RFM69_TARGETID = 0;
    RFM69_PAYLOADLEN = 0;
    RFM69_ACK_REQUESTED = 0;
    RFM69_ACK_RECEIVED = 0;
    RFM69_RSSI = 0;
    if (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY)
        RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
    RFM69_writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
    RFM69_setMode(RF69_MODE_RX);
}

// checks if a packet was received and/or puts transceiver in receive (ie RX or listen) mode

unsigned char RFM69_receiveDone() {
    //ATOMIC_BLOCK(ATOMIC_FORCEON)
    //{
    if (_haveData) {
        _haveData = FALSE;
        RFM69_interruptHandler();
    }
    if (RFM69_mode == RF69_MODE_RX && RFM69_PAYLOADLEN > 0) {
        RFM69_setMode(RF69_MODE_STANDBY); // enables interrupts
        return TRUE;
    } else if (RFM69_mode == RF69_MODE_RX) // already in RX no payload yet
    {
        return FALSE;
    }
    RFM69_receiveBegin();
    return FALSE;
    //}
}

// To enable encryption: radio.encrypt("ABCDEFGHIJKLMNOP");
// To disable encryption: radio.encrypt(null) or radio.encrypt(0)
// KEY HAS TO BE 16 bytes !!!

void RFM69_encrypt(const char* key) {
    RFM69_setMode(RF69_MODE_STANDBY);
    if (key != 0) {
        RFM69_CS_LAT = FALSE;
        SPI2Transfer(REG_AESKEY1 | 0x80);
        for (unsigned char i = 0; i < 16; i++)
            SPI2Transfer(key[i]);
        RFM69_CS_LAT = TRUE;
    }
    RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFE) | (key ? 1 : 0));
}

// get the received signal strength indicator (RSSI)

signed int RFM69_readRSSI() {
    unsigned char forceTrigger = FALSE;
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

// TRUE  = disable filtering to capture all frames on network
// FALSE = enable node/broadcast filtering to capture only frames sent to this/broadcast address

void RFM69_promiscuous(unsigned char onOff) {
    RFM69_promiscuousMode = onOff;
    //RFM69_writeReg(REG_PACKETCONFIG1, (RFM69_readReg(REG_PACKETCONFIG1) & 0xF9) | (onOff ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST));
}

// for RFM69HW only: you must call setHighPower(TRUE) after initialize() or else transmission won't work

void RFM69_setHighPower(unsigned char onOff) {
    _isRFM69HW = onOff;
    RFM69_writeReg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
    if (_isRFM69HW) // turning ON
        RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
    else
        RFM69_writeReg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | _powerLevel); // enable P0 only
}

// internal function

void RFM69_setHighPowerRegs(unsigned char onOff) {
    RFM69_writeReg(REG_TESTPA1, onOff ? 0x5D : 0x55);
    RFM69_writeReg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

void RFM69_rcCalibration() {
    RFM69_writeReg(REG_OSC1, RF_OSC1_RCCAL_START);
    while ((RFM69_readReg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00);
}

void RFM69_writeReg(unsigned char address, unsigned char data){
    address |= 0b10000000; //Set MSB to 1 for write
    RFM69_CS_LAT = FALSE; //select radio
    SPI2Transfer(address);
    SPI2Transfer(data);
    RFM69_CS_LAT = TRUE; //Deselect radio
}

unsigned char RFM69_readReg(unsigned char address){
    address &= 0b01111111; //Set MSB to 0 for read
    RFM69_CS_LAT = FALSE; //select radio
    SPI2Transfer(address);
    unsigned char result = SPI2Transfer(0);
    RFM69_CS_LAT = TRUE; //Deselect radio
    return result;
}

//=============================================================================
//                     ListenMode specific functions  
//=============================================================================
#if defined(RF69_LISTENMODE_ENABLE)
volatile unsigned int RFM69_RF69_LISTEN_BURST_REMAINING_MS = 0;

//=============================================================================
// reinitRadio() - use base class initialization with saved values
//=============================================================================

unsigned char RFM69_reinitRadio() {
    if (!initialize(_freqBand, _address, _networkID)) return FALSE;
    if (_haveEncryptKey) RFM69_encrypt(_encryptKey); // Restore the encryption key if necessary
    if (_isHighSpeed) RFM69_writeReg(REG_LNA, (RFM69_readReg(REG_LNA) & ~0x3) | RF_LNA_GAINSELECT_AUTO);
    return TRUE;
}

static unsigned long getUsForResolution(unsigned char resolution) {
    switch (resolution) {
        case RF_LISTEN1_RESOL_RX_64:
        case RF_LISTEN1_RESOL_IDLE_64:
            return 64;
        case RF_LISTEN1_RESOL_RX_4100:
        case RF_LISTEN1_RESOL_IDLE_4100:
            return 4100;
        case RF_LISTEN1_RESOL_RX_262000:
        case RF_LISTEN1_RESOL_IDLE_262000:
            return 262000;
        default:
            // Whoops
            return 0;
    }
}

static unsigned long getCoefForResolution(unsigned char resolution, unsigned long duration) {
    unsigned long resolDuration = getUsForResolution(resolution);
    unsigned long result = duration / resolDuration;

    // If the next-higher coefficient is closer, use that
    if (abs(duration - ((result + 1) * resolDuration)) < abs(duration - (result * resolDuration)))
        return result + 1;

    return result;
}

static unsigned char chooseResolutionAndCoef(unsigned char *resolutions, unsigned long duration, unsigned char& resolOut, unsigned char& coefOut) {
    for (int i = 0; resolutions[i]; i++) {
        unsigned long coef = getCoefForResolution(resolutions[i], duration);
        if (coef <= 255) {
            coefOut = coef;
            resolOut = resolutions[i];
            return TRUE;
        }
    }

    // out of range
    return FALSE;
}

#endif