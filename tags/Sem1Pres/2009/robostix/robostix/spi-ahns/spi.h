

#ifndef __SPI_H
#define __SPI_H

#include <avr/io.h>

#include <stdio.h>
#include <avr/eeprom.h>

#include "Timer.h"
#include "Config.h"
#include "Delay.h"
#include "Hardware.h"
#include "UART.h"
#include "spi.h"
#include "common.h"

#define GYRO_SCALE 0.07326
#define XGYRO_OUT 0x04
#define YGYRO_OUT 0x06
#define ZGYRO_OUT 0x08

#define ACC_SCALE 2.522
#define XACC_OUT 0x0A
#define YACC_OUT 0x0C
#define ZACC_OUT 0x0E

#define SUPPLY_OUT 0x02

void spi_master_init(void);
void spi_slave_init(void);
uint16_t spi_transmit(uint16_t data);
char  spi_slave_receive(void);

#endif
