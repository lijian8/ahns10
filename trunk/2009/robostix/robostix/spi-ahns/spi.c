#include "spi.h"



void spi_master_init() {


  /* Check whether PB0 is an input. */
    if (bit_is_clear(DDRB, 0)) {
        /* If yes, activate the pull-up. */
        PORTB |= _BV(0);
    }


  // set SCK and MOSI as output (SS/PB0 must be set as output, when SPI is master)
  DDRB |= (_BV(PB1) | _BV(PB2));
  
  // use PB0 as SS
  PORTB |= _BV(PB3);
  
  // enable spi, set as master, scale sck freq with 128
  // SPCR |= (_BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0));
  
  // enable spi, set as master, scale sck freq with 4
  SPCR |= (_BV(SPE) | _BV(MSTR));
  
  // double SPI speed
  //SPSR |= _BV(SPI2X);
}


uint16_t spi_transmit(uint16_t data) {
	// set SS low
	PORTB &= ~_BV(PB0);
	
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	
	// set SS high
	PORTB |= _BV(PB0);
	
	return SPDR;
}


void spi_slave_init(void){



  DDRB |= _BV(PB3);

  // enable spi, set as master, scale sck freq with 4
  SPCR |= (_BV(SPE) | _BV(MSTR));

}


char spi_slave_receive(){

  /* wait for reception complete */
  while(!(SPSR & (1 << SPIF)))
    ;
  
  /* return data register */
  return SPDR;



}



int main( void )
{
  //char data;
  signed short xacc=0;
  uint16_t yacc=0;
  uint16_t zacc=0;

  uint16_t xgyro=0;
  uint16_t ygyro=0;
  uint16_t zgyro=0;

  uint16_t status_low=0, status_high=0;
  int loop=0;


   // unsigned char rcv[ 8 ]={0x00}; 

  InitHardware();
  InitTimer();
  
  ASSR &= ~( 1 << AS0 );  // Make sure Port G LED pins are setup for I/O
  
  RED_LED_DDR     |= RED_LED_MASK;
  YELLOW_LED_DDR  |= YELLOW_LED_MASK;
  BLUE_LED_DDR    |= BLUE_LED_MASK;
  
  /* init SPI */
  spi_master_init();
 
  LED_OFF( RED );
  
  // The first handle opened for read goes to stdin, and the first handle
  // opened for write goes to stdout. So u0 is stdin, stdout, and stderr
  
#if defined( __AVR_LIBC_VERSION__ )
  fdevopen( UART0_PutCharStdio, UART0_GetCharStdio );
#else
  fdevopen( UART0_PutCharStdio, UART0_GetCharStdio, 0 );
#endif

  //unsigned char *bp;// = malloc(8);// = (unsigned char *)buffer;
 

    while ( 1 )
    {
      loop++;
       
       LED_OFF( RED );
       Delay100mSec(1);
      /* read SPI */
       //data=spi_slave_receive(); 
       
       status_high = spi_transmit(0x3D);
       status_low = spi_transmit(0x3C);

       xacc = spi_transmit(XACC_OUT); 
       yacc = spi_transmit(YACC_OUT);
       zacc = spi_transmit(ZACC_OUT);

       xgyro = spi_transmit(XGYRO_OUT); 
       ygyro = spi_transmit(YGYRO_OUT);
       zgyro = spi_transmit(ZGYRO_OUT);


       printf("loop:%d \n", loop);
       printf("acc x:%x y:%x z:%x \n", xacc, yacc, zacc);
       printf("gyro x:%x y:%x z:%x \n",xgyro,ygyro,zgyro);
       printf("status: %x%x \n",status_low,status_high);

      /* process SPI */

      /* send to the serial port */
       //putchar('t');
       putchar('\n');	  

       LED_ON( RED );

       Delay100mSec(2);

      
      int i;
       for ( i = 0; i < 100; i++ ) 
            {
             WaitForTimer0Rollover();
            }
 


    } 
	

    

} // main
