#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include the library for the RFM12 module and the UART
#include "rfm12.h"

char uart_buff[100] = ""; //uart input buffer
uint8_t uart_index = 0;	  //uart input string position

char rx_buff[100] = "";

void tx_msg(char *message)
{
	// Determine the length of the string
	uint8_t length = 0;
	while (message[length] != '\0')
	{
		length++;
	}

	// Queue message for transmission on rmf12 module
	rfm12_tx(length, 0xEE, (uint8_t *)message);

	// Tick the device to transmit
	rfm12_tick();

	// Give the user an acknowledgement over UART
	// put_str("\r\n[i] message transmitted: ");
	// put_str(message);
	// put_str("\r\n\r\n");
}

void rx_poll()
{
	if (rfm12_rx_status() == STATUS_COMPLETE)
	{
		// Determine the length of the incoming data
		uint8_t rx_length = rfm12_rx_len();

		// Quick sanity check to ensure we are receiving good data
		if ((rx_length == 0) || (rx_length > 100))
		{
			// Malformed data
			return;
		}

		// Receive the data
		memcpy(rx_buff, rfm12_rx_buffer(), rx_length);

		// Clear the chip buffer after we read it
		rfm12_rx_clear();

		// put_str("message received: ");
		// put_str(rx_buff);
		// put_str("\n\r");
	}
}

int main(void)
{
	init_uart0();	//init uart
	_delay_ms(100); //delay for the rfm12 to initialize properly
	rfm12_init();	//init the RFM12
	_delay_ms(100);
	sei(); //interrupts on

	while (1)
	{
		rx_poll();

		rfm12_tick();
		_delay_ms(250);
	}
}

// void uart_input(char byte)
// {
// 	// Show the user what they are typing
// 	put_ch(byte);

// 	// Store the character in the input buffer
// 	uart_buff[uart_index] = byte;

// 	// Determine if the return key has been hit
// 	if (byte == '\r')
// 	{
// 		// Reset the input buffer pointer
// 		uart_index = 0;

// 		// Transmit the entire input string
// 		tx_msg(uart_buff);
// 	}
// 	// Ensure TX string is under 100 characters
// 	else if (uart_index < 100)
// 	{
// 		// Select the next character in the string as long
// 		// as the string is under 100 characters long
// 		uart_index++;
// 	}
// 	else
// 	{
// 		// Overwrite the existing string when the input is
// 		// greater than 100 characters so we don't lose it
// 		uart_index = 0;
// 	}
// }

// /* UART byte received interrupt */
// ISR(USART0_RX_vect)
// {
// 	uart_input(UDR0);
// 	_delay_ms(0);
// }
