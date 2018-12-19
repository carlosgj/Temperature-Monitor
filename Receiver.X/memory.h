#ifndef MEMORY_H
#define	MEMORY_H

#include <xc.h> 

#define DISABLE_EXTERNAL_MEMORY_WRITE
//#define DISABLE_INTERNAL_MEMORY_WRITE

#define INTERNAL_EEPROM_PAGE_INDEX_LOCATION 0
#define INTERNAL_EEPROM_PAGE_INDEX_COMPLEMENT_LOCATION 2

#define EXT_MEM_OP_WRITE    (2)
#define EXT_MEM_OP_READ     (3)
#define EXT_MEM_OP_WREN     (6)
#define EXT_MEM_OP_WRDI     (4)


void int_mem_write(unsigned int address, unsigned char val);
unsigned char int_mem_read(unsigned int address);

void ext_mem_write(unsigned short long address, unsigned char val);
unsigned char ext_mem_read(unsigned short long address);

unsigned char loadEEPROMPageIndex(void);
void incrementEEPROMPageIndex(void);
void resetEEPROMPageIndex(void);
void setupEEPROMPage(unsigned int pageIndex, unsigned char year, unsigned char month, unsigned char day);
unsigned int formatDateToDatestamp(unsigned char year, unsigned char month, unsigned char day);
void dumpExtMemPage(unsigned int page); 
void fillExtMemPage(unsigned int page);
unsigned int currentEEPROMPage = 0xffff;

#endif	/* XC_HEADER_TEMPLATE_H */

