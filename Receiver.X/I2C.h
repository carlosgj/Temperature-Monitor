#ifndef I2C_H
#define	I2C_H

#include <xc.h> 
#include "common.h"
#include "pindef.h"

uint8_t I2C_init(void);
uint8_t I2C_read(uint8_t address, uint8_t *data, uint8_t count);
uint8_t I2C_write(uint8_t address, uint8_t *data, uint8_t count);

#endif	/* XC_HEADER_TEMPLATE_H */

