/*
 * $Id: spi_hw.c,v 1.11 2006/07/01 17:22:43 poine Exp $
 *  
 * Copyright (C) 2003-2005  Pascal Brisset, Antoine Drouin
 *
 * This file is part of paparazzi.
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA. 
 *
 */

/** \file spi_hw.c
 *  \brief handling of hardware dependant SPI on AVR architecture
 */



#include "spi.h"

#define  SPI_SLAVE

#define SPI_DDR  DDRB
#define PORT_SPI PORTB
#define PIN_SS   0
#define PIN_SCK  1
#define PIN_MOSI 2
#define PIN_MISO 4
#define CLOCK 16000000

#include <inttypes.h>
#include <avr/io.h>

#if (__GNUC__ == 3)
#include <avr/signal.h>
#endif

#include <avr/interrupt.h>

volatile uint8_t spi_idx_buf;

#define HandleOneSpiByte() {				\
    spi_idx_buf++;		\
    if (spi_idx_buf < spi_buffer_length) {		\
      SPDR = spi_buffer_output[spi_idx_buf];		\
      spi_buffer_input[spi_idx_buf-1] = SPDR;		\
    } else if (spi_idx_buf == spi_buffer_length) {	\
      spi_buffer_input[spi_idx_buf-1] = SPDR;		\
      spi_message_received = TRUE;			\
      SpiStop();					\
    }							\
  }


#ifdef SPI_SLAVE

volatile bool_t spi_was_interrupted = FALSE;

void spi_init(void) {
  /* set it pin output */
  //  IT_DDR |= _BV(IT_PIN);

  /* set MISO pin output */
  SLAVE_SPI_DDR |= _BV(SLAVE_SPI_MISO_PIN);
  /* enable SPI, slave, MSB first, sck idle low */
  SPCR = _BV(SPE);
  /* enable interrupt */
  SPCR |= _BV(SPIE);
}

#define SpiStop() {}


SIGNAL(SIG_SPI) {
  HandleOneSpiByte();
}

#endif /** SPI_SLAVE */


/****************************************************************************/
#ifdef SPI_MASTER

//#include "autopilot.h"

#define SpiStop() { \
  ClearBit(SPCR,SPIE); \
  ClearBit(SPCR, SPE); \
  SpiUnselectAllSlaves(); \
}

volatile uint8_t spi_cur_slave;
uint8_t spi_nb_ovrn;



void spi_init( void) {
  /* Set MOSI and SCK output, all others input */ 
  SPI_DDR |= _BV(PIN_MOSI)| _BV(PIN_SCK); 

  /* enable pull up for miso */
  /*  SPI_PORT |= _BV(MASTER_SPI_MISO_PIN);*/

  /* Set SS0 output */
  SetBit( PORT_SPI, PIN_SS);
  /* SS0 idles high (don't select slave yet)*/

#if 0
  /* Set SS1 output */
  /* SetBit( MASTER_SPI_SS1_DDR, MASTER_SPI_SS1_PIN);*/
  /* SS1 idles high (don't select slave yet)*/
  
  /* Set SS2 output */
  /* SetBit( MASTER_SPI_SS2_DDR, MASTER_SPI_SS2_PIN);*/
  /* SS2 idles high (don't select slave yet)*/
#endif

  SpiUnselectAllSlaves();

  spi_cur_slave = SPI_NONE;
}


/** SPI interrupt: starts a delay */
SIGNAL(SIG_SPI) {
  /*  if (spi_cur_slave == SPI_SLAVE0) { */
  /* setup OCR1C to pop in 200 clock cycles */
  /* this leaves time for the slave (fbw) */
  /* to process the byte we've sent and to  */
  /* prepare a new one to be sent           */
  OCR1C = TCNT1 + (200UL*CLOCK)/16;
  /* clear interrupt flag  */
  SetBit(ETIFR, OCF1C);
  /* enable OC1C interrupt */
  SetBit(ETIMSK, OCIE1C);
  /*  } else
      fatal_error_nb++;
  */
}

/** Send a byte */
SIGNAL(SIG_OUTPUT_COMPARE1C) {
  /* disable OC1C interrupt */
  ClearBit(ETIMSK, OCIE1C);

  HandleOneSpiByte();
}

#endif /* SPI_MASTER */
