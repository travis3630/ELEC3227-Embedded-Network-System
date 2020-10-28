#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include the library for the RFM12 module and the UART
#include "rfm12.h"

uint8_t *bufptr;
char str[100];	//the string want to send
uint16_t p = 0;	//the position of string

int main(void)
{
	init_uart0();    //init uart
	_delay_ms(100);  //delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	uint8_t tv[] = "pippo\n\r"; //the message to send
	sei();           //interrupts on

   while(1)
   {
         put_str("write here your code \n\r");
   }
}
