#include <xc.h>
#include "memory.h"
#include "common.h"
#include "display.h"
#include "pindef.h"
#include "SPI.h"

void int_mem_write(unsigned int address, unsigned char val){
#ifndef DISABLE_INTERNAL_MEMORY_WRITE
    NVMCON1bits.NVMREG = 0; //Select data EEPROM
    NVMCON1bits.WREN = TRUE; //Enable write
    NVMADR = address;
    NVMDAT = val;
    INTCONbits.GIE = FALSE; //Disable interrupts during write sequence
    NVMCON2 = 0x55;
    NVMCON2 = 0xaa;
    NVMCON1bits.WR = TRUE; //Start write
    while(NVMCON1bits.WR){
        //Wait for write to complete
    }
    NVMCON1bits.WREN = FALSE; //Diable writes
    INTCONbits.GIE = TRUE; //Reenable interrupts    
#endif
}


unsigned char int_mem_read(unsigned int address){
    NVMCON1bits.NVMREG = 0; //Select data EEPROM
    NVMADR = address;
    NVMCON1bits.RD = TRUE; //Initiate read
    return NVMDAT;
}

void ext_mem_write(unsigned short long address, unsigned char val){
    if((address & 0x20000) == 0){
        address &= 0x1ffff;
        //First enable write latch
        MEM1_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_WREN);
        MEM1_CS_LAT = TRUE;
        __delay_us(10);
        
        MEM1_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_WRITE);
        SPI2Transfer((unsigned char)(address >> 16));
        SPI2Transfer((unsigned char)(address >> 8));
        SPI2Transfer((unsigned char)(address));
        SPI2Transfer(val);
        MEM1_CS_LAT = TRUE;
    }
    else{
        address &= 0x1ffff;
        //First enable write latch
        MEM2_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_WREN);
        MEM2_CS_LAT = TRUE;
        __delay_us(10);
        
        MEM2_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_WRITE);
        SPI2Transfer((unsigned char)(address >> 16));
        SPI2Transfer((unsigned char)(address >> 8));
        SPI2Transfer((unsigned char)(address));
        SPI2Transfer(val);
        MEM2_CS_LAT = TRUE;
    }
    __delay_ms(7);
}

unsigned char ext_mem_read(unsigned short long address){
    unsigned char result = 0xaa;
    if((address & 0x20000) == 0){
        address &= 0x1ffff;
        MEM1_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_READ);
        SPI2Transfer((unsigned char)(address >> 16));
        SPI2Transfer((unsigned char)(address >> 8));
        SPI2Transfer((unsigned char)(address));
        result = SPI2Transfer(0);
        MEM1_CS_LAT = TRUE;
    }
    else{
        address &= 0x1ffff;
        MEM2_CS_LAT = FALSE;
        SPI2Transfer(EXT_MEM_OP_READ);
        SPI2Transfer((unsigned char)(address >> 16));
        SPI2Transfer((unsigned char)(address >> 8));
        SPI2Transfer((unsigned char)(address));
        result = SPI2Transfer(0);
        MEM2_CS_LAT = TRUE;
    }
    return result;
}

void dumpExtMemPage(unsigned int page){
    RA8875_fillScreen(RA8875_BLACK);
    RA8875_textMode();
    RA8875_textEnlarge(0);
    RA8875_textTransparent(RA8875_WHITE);
    
    unsigned char val;
    unsigned char byteIndex;
    unsigned short long address;
    char chars[2];
    unsigned char i; 
    unsigned char j;
    for(i = 0; i<16; i++){
        for(j=0; j<16; j++){
            byteIndex = (i<<4) | j;
            address = page;
            address <<= 8;
            address |= byteIndex;
            val = ext_mem_read(address);
            itoh8(val, chars);
            RA8875_textWrite(chars, 2);
            PRINT(" ");
        }
        PRINT("\n");
    }
    
}

void fillExtMemPage(unsigned int page){
    unsigned char val;
    unsigned char byteIndex;
    unsigned short long address;
    unsigned char i; 
    unsigned char j;
    for(i = 0; i<16; i++){
        for(j=0; j<16; j++){
            byteIndex = (i<<4) | j;
            address = page;
            address <<= 8;
            address |= byteIndex;
            ext_mem_write(address, byteIndex);
        }
    }
}

unsigned char loadEEPROMPageIndex(void){
    char uintStr[4];
    //Read internal EEPROM 
    unsigned int intmemindex = ((unsigned int)int_mem_read(INTERNAL_EEPROM_PAGE_INDEX_LOCATION) << 8) | int_mem_read(INTERNAL_EEPROM_PAGE_INDEX_LOCATION+1);
    unsigned int intmemindex_c = ((unsigned int)int_mem_read(INTERNAL_EEPROM_PAGE_INDEX_COMPLEMENT_LOCATION) << 8) | int_mem_read(INTERNAL_EEPROM_PAGE_INDEX_COMPLEMENT_LOCATION+1);
    //Validate internal EEPROM
    if(intmemindex != (~intmemindex_c)){
        PRINT("NVM stored EEPROM page index invalid!\n");
        PRINT("Index: 0x");
        itoh16(intmemindex, uintStr);
        RA8875_textWrite(uintStr, 4);
        PRINT("\n");
        PRINT("Complementary index: 0x");
        itoh16(intmemindex_c, uintStr);
        RA8875_textWrite(uintStr, 4);
        PRINT("\n");
        return TRUE;
    }
    PRINT("EEPROM page index from internal memory: 0x");
    
    itoh16(intmemindex, uintStr);
    RA8875_textWrite(uintStr, 4);
    PRINT("\n");
    //Check datestamp on EEPROM page indicated by stored index
    unsigned int currentDatestamp = formatDateToDatestamp(currentYear, currentMonth, currentDay);
    unsigned short long pageheaderaddress = intmemindex << 8;
    pageheaderaddress &= 0x3ffff; //For safety, zero out leading 6 bits
    //Check if datestamp matches current datestamp
    unsigned int pagedatestamp = (ext_mem_read(pageheaderaddress) << 8) | ext_mem_read(pageheaderaddress+1);
    pagedatestamp &= 0x7f; //Zero out first bit
    current_EEPROM_page = intmemindex;
    if(pagedatestamp == currentDatestamp){
        return FALSE;
    }
    else{
        if(pagedatestamp < currentDatestamp){
            incrementEEPROMPageIndex();
            return FALSE;
        }
        if(pagedatestamp > currentDatestamp){
            PRINT("Current date earlier than header of indicated page in NVM!\n");
            return TRUE;
        }
    }
    return TRUE; //Should never be reached
}

void incrementEEPROMPageIndex(void){
    //Check if we need to roll over
    //Change value in RAM
    //Write to internal EEPROM
    //Setup new page
    //close out old page, if necessary
}

void resetEEPROMPageIndex(void){
    unsigned int newPageIndex = 0;
    unsigned int newPageIndex_c = ~newPageIndex;
    int_mem_write(INTERNAL_EEPROM_PAGE_INDEX_LOCATION, (unsigned char)(newPageIndex>>8));
    int_mem_write(INTERNAL_EEPROM_PAGE_INDEX_LOCATION+1, (unsigned char)(newPageIndex));
    int_mem_write(INTERNAL_EEPROM_PAGE_INDEX_COMPLEMENT_LOCATION, (unsigned char)(newPageIndex_c>>8));
    int_mem_write(INTERNAL_EEPROM_PAGE_INDEX_COMPLEMENT_LOCATION+1, (unsigned char)(newPageIndex_c));
    
    //Setup new page
    setupEEPROMPage(newPageIndex, currentYear, currentMonth, currentDay);
    
    //Change value in RAM
    current_EEPROM_page = newPageIndex;
}

void setupEEPROMPage(unsigned int pageIndex, unsigned char year, unsigned char month, unsigned char day){
    unsigned int datestamp = formatDateToDatestamp(year, month, day);
    //Write to EEPROM page header
    unsigned short long pageheaderaddress = pageIndex << 8;
    pageheaderaddress &= 0x3ffff; //For safety, zero out leading 6 bits
    ext_mem_write(pageheaderaddress, ((unsigned char)(datestamp>>8))&0b01111111);
    ext_mem_write(pageheaderaddress, (unsigned char)(datestamp & 0xff));
}

unsigned int formatDateToDatestamp(unsigned char year, unsigned char month, unsigned char day){
    unsigned int result = 0;
    result |= (((unsigned int)year & 0b00111111) << 9);
    result |= (((unsigned int)month & 0b00001111) << 5);
    result |= (day & 0b00011111);
    return result;
}