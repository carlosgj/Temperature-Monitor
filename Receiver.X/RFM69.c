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

volatile uint8_t RFM69_haveData;

uint8_t RFM69_promiscuousMode = TRUE;
uint8_t RFM69_powerLevel = 31;

uint8_t RFM69_initialize(uint8_t nodeID, uint8_t networkID) {
    uint8_t CONFIG[][2] = {
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
        { REG_SYNCVALUE2, networkID}, // NETWORK ID
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

    uint16_t start, now;
    uint8_t timeout = 50;
    uint8_t result = 0;

    RFM69_CS_TRIS = OUTPUT;
    SPI2_Open_RFM69();

    getMillis(&start);
    while (TRUE) {
        RFM69_writeReg(REG_SYNCVALUE1, 0xAA);
        __delay_ms(1);
        if (RFM69_readReg(REG_SYNCVALUE1) == 0xaa) {
            break;
        }
        getMillis(&now);
        if (now > (start + timeout)) {
            printf("\tRF: AA sync timeout!\n");
            result = 1;
            break;
        }
    }

    getMillis(&start);
    while (TRUE) {
        RFM69_writeReg(REG_SYNCVALUE1, 0x55);
        __delay_ms(1);
        if (RFM69_readReg(REG_SYNCVALUE1) == 0x55) {
            break;
        }
        getMillis(&now);
        if (now > (start + timeout)) {
            printf("\tRF: 55 sync timeout!\n");
            result = 1;
            break;
        }
    }

    if (result == 0) {
        printf("\tRF: Writing initial config\n");
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
        RFM69_setHighPower(RF_isRFM69HW); // called regardless if it's a RFM69W or RFM69HW
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
    TRISCbits.TRISC7 = INPUT;
    WPUCbits.WPUC7 = TRUE;
    IOCCPbits.IOCCP7 = TRUE;
    PIE0bits.IOCIE = TRUE;

    RF_address = nodeID;
    
    return result;
}

// return the frequency (in Hz)

unsigned long RFM69_getFrequency() {
    return (unsigned long) (RF69_FSTEP * (((unsigned long) RFM69_readReg(REG_FRFMSB) << 16) + ((unsigned int) RFM69_readReg(REG_FRFMID) << 8) + RFM69_readReg(REG_FRFLSB)));
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
            if (RF_isRFM69HW) RFM69_setHighPowerRegs(TRUE);
            break;
        case RF69_MODE_RX:
            RFM69_writeReg(REG_OPMODE, (RFM69_readReg(REG_OPMODE) & 0xE3) | RF_OPMODE_RECEIVER);
            if (RF_isRFM69HW) RFM69_setHighPowerRegs(FALSE);
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

void RFM69_setAddress(uint8_t addr) {
    RF_address = addr;
    RFM69_writeReg(REG_NODEADRS, RF_address);
}

//set this node's network id

void RFM69_setNetwork(uint8_t networkID) {
    RFM69_writeReg(REG_SYNCVALUE2, networkID);
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
    if (RF_isRFM69HW) {
        RF_powerLevel /= 2;
    }
    RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0xE0) | RF_powerLevel);
}

uint8_t RFM69_canSend() {
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
        if (!(RFM69_promiscuousMode || RFM69_TARGETID == RF_address || RFM69_TARGETID == RF69_BROADCAST_ADDR) // match this node's address, or broadcast address or anything in promiscuous mode
                || RFM69_PAYLOADLEN < 3) // address situation could receive packets that are malformed and don't fit this libraries extra fields
        {
            RFM69_PAYLOADLEN = 0;
            RFM69_CS_LAT = TRUE;
            RFM69_receiveBegin();
            return;
        }

        RFM69_DATALEN = RFM69_PAYLOADLEN - 3;
        RFM69_SENDERID = SPI2Transfer(0);
        uint8_t CTLbyte = SPI2Transfer(0);

        RFM69_ACK_RECEIVED = CTLbyte & RFM69_CTL_SENDACK; // extract ACK-received flag
        RFM69_ACK_REQUESTED = CTLbyte & RFM69_CTL_REQACK; // extract ACK-requested flag

        //interruptHook(CTLbyte); // TWS: hook to derived class interrupt function

        for (uint8_t i = 0; i < RFM69_DATALEN; i++) {
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

void RFM69_receiveBegin() {
    RFM69_DATALEN = 0;
    RFM69_SENDERID = 0;
    RFM69_TARGETID = 0;
    RFM69_PAYLOADLEN = 0;
    RFM69_ACK_REQUESTED = 0;
    RFM69_ACK_RECEIVED = 0;
    RFM69_RSSI = 0;
    if (RFM69_readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PAYLOADREADY){
        RFM69_writeReg(REG_PACKETCONFIG2, (RFM69_readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
    }
    RFM69_writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_01); // set DIO0 to "PAYLOADREADY" in receive mode
    RFM69_setMode(RF69_MODE_RX);
}

// checks if a packet was received and/or puts transceiver in receive (ie RX or listen) mode

uint8_t RFM69_receiveDone() {
    if (RF_haveData) {
#ifdef DEBUG_RF
        printf("RF: Have data!\n");
#endif
        RF_haveData = FALSE;
        RFM69_interruptHandler();
    }
    if (RFM69_mode == RF69_MODE_RX && RFM69_PAYLOADLEN > 0) {
#ifdef DEBUG_RF
        printf("RF: Going to standby\n");
#endif
        RFM69_setMode(RF69_MODE_STANDBY); // enables interrupts
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

// TRUE  = disable filtering to capture all frames on network
// FALSE = enable node/broadcast filtering to capture only frames sent to this/broadcast address

void RFM69_promiscuous(uint8_t onOff) {
    RFM69_promiscuousMode = onOff;
    //RFM69_writeReg(REG_PACKETCONFIG1, (RFM69_readReg(REG_PACKETCONFIG1) & 0xF9) | (onOff ? RF_PACKET1_ADRSFILTERING_OFF : RF_PACKET1_ADRSFILTERING_NODEBROADCAST));
}

// for RFM69HW only: you must call setHighPower(TRUE) after initialize() or else transmission won't work

void RFM69_setHighPower(uint8_t onOff) {
    RF_isRFM69HW = onOff;
    RFM69_writeReg(REG_OCP, RF_isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
    if (RF_isRFM69HW) { // turning ON
        RFM69_writeReg(REG_PALEVEL, (RFM69_readReg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
    } else {
        RFM69_writeReg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | RF_powerLevel); // enable P0 only
    }
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
    printf("RF: Wrote %02X to %02X\n", data, address&0b01111111);
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
    printf("RF: Read %02X from %02X\n", result, address&0b01111111);
#endif
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

uint8_t RFM69_reinitRadio() {
    if (!initialize(_freqBand, _address, _networkID)) return FALSE;
    if (_haveEncryptKey) RFM69_encrypt(_encryptKey); // Restore the encryption key if necessary
    if (_isHighSpeed) RFM69_writeReg(REG_LNA, (RFM69_readReg(REG_LNA) & ~0x3) | RF_LNA_GAINSELECT_AUTO);
    return TRUE;
}

static unsigned long getUsForResolution(uint8_t resolution) {
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

static unsigned long getCoefForResolution(uint8_t resolution, unsigned long duration) {
    unsigned long resolDuration = getUsForResolution(resolution);
    unsigned long result = duration / resolDuration;

    // If the next-higher coefficient is closer, use that
    if (abs(duration - ((result + 1) * resolDuration)) < abs(duration - (result * resolDuration)))
        return result + 1;

    return result;
}

static uint8_t chooseResolutionAndCoef(uint8_t *resolutions, unsigned long duration, uint8_t& resolOut, uint8_t& coefOut) {
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