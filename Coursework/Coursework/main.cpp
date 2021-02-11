//#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string.h>
//#include <util/delay.h>
//#include <avr/interrupt.h>

//#include the library for the RFM12 module and the UART
//#include "rfm12.h"
//#include "DLLnPHY.h"
using namespace std;

uint8_t node_addr = 1;
uint8_t HeaderFooter = 0x7E;
uint8_t flagbyte = 0x55;
uint8_t NET_packet[128] = {
    0x74, 0x64,
    0x01, 0x0F,   // src and dest addr
    0x79,         // length
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x6E, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
    0x5A, // Trans segment x121
    0x7E, 0x4C
};      // an example of NET packet

char uart_buff[100] = ""; //uart input buffer
uint8_t uart_index = 0;	  //uart input string position

char rx_buff[100] = "";

typedef struct Packet{
    uint8_t Header;  // 1 byte
    uint8_t Footer;  // 1 byte
    uint8_t Netpkt[23];       // 1-23bytes
    uint8_t checksum[2];    // 2 bytes
    uint8_t Length;         // 1 byte
    uint8_t Address[2];     // 2 bytes
    uint8_t Control[2];     // 2 bytes
    uint8_t Everything[];   // bytes
} Packet;

Packet checksum(Packet Payload)
{
	uint8_t length =23;
	// while(Payload.Netpkt[length] != 0)
	// {
	// 	length++;
	// }
	//cout<<dec <<(int)length << endl;
    uint16_t A[length];
    uint16_t B[length];
    uint16_t accumulate = 0;
	uint16_t accumulate_of_A = 0;
    for(uint8_t i=0;i<length;i++)
    {
        accumulate += Payload.Netpkt[i];
        A[i] = accumulate;
		// cout <<dec << (int)A[i] << " ";
        accumulate_of_A += A[i];
        B[i] = accumulate_of_A;
    }
	// cout << endl;
    Payload.checksum[0] = A[length-1]%256;
    Payload.checksum[1] = B[length-1]%256; //  modulus 256 because 1 byte is 2**8 bits
    return Payload;
}

bool errorchecking(Packet errorcheck)
{
	uint8_t length =23;
	// while(errorcheck.Netpkt[length] != 0)
	// {
	// 	length++;
	// }
	//cout<<dec <<(int)length << endl;
    uint16_t A[length];
    uint16_t B[length];
	uint16_t accumulate = 0;
	uint16_t accumulate_of_A = 0;
    for(uint8_t i=0;i<length;i++)
    {
        accumulate += errorcheck.Netpkt[i];
        A[i] = accumulate;
		//cout <<dec << (int)A[i] << " ";
        accumulate_of_A += A[i];
        B[i] = accumulate_of_A;
    }
    if(errorcheck.checksum[0]==(A[length-1]%256) || errorcheck.checksum[1]==(B[length-1]%256))
    {return true;}
    else
    {return false;}
}


int main(void)
{
	// init_uart0();	//init uart
	// _delay_ms(100); //delay for the rfm12 to initialize properly
	// rfm12_init();	//init the RFM12
	// _delay_ms(100);
	// sei(); //interrupts on

	// while (1)
	// {
		
	// }
	uint8_t length =0;
	Packet Payload;
	Packet Mutated;
	uint8_t values[24] = {0x74, 0x64,
    0x01, 0x0F,   // src and dest addr
    0x79,         // length
    0x5A, 0x5A, 0x5A, 0x5A, 0x45, 0x5A, 0x6B, 0x5A, 0x78, 0x5A, 0x3A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x7E, 0x01, 0x00};
	while(values[length] != 0)
	{
		length++;
	}
	memcpy(Payload.Netpkt, values, length);
	cout<< "Net packt value: " ;
	for(uint8_t i=0;i<length;i++)
	{
		cout << hex <<(int)Payload.Netpkt[i] << " ";
	}
	cout << endl<< "string length: "<<dec <<(int)length <<endl;
	Payload = checksum(Payload);
	cout <<"checksum bytes: ";
	for(uint8_t i=0;i<2;i++)
	{
		cout << hex <<(int)Payload.checksum[i] << " " ;
	}
	cout << endl << "when Payload is used to check back itself:";
	cout << endl<<"result of errorchecking: "<<errorchecking(Payload)<< endl;

	uint8_t values2[24] = {0x74, 0x64,
    0x01, 0x0F,   // src and dest addr
    0x79,         // length
    0x5A, 0x5A, 0x5A, 0x01, 0x99, 0x5A, 0x6B, 0x5A, 0x78, 0x5A, 0x3A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x7E, 0x01, 0x00};
	while(values[length] != 0)
	{
		length++;
	}
	memcpy(Mutated.Netpkt, values2, length);
	cout<< " Mutated Net packt value: " ;
	for(uint8_t i=0;i<length;i++)
	{
		cout << hex <<(int)Mutated.Netpkt[i] << " ";
	}
	Mutated.checksum[0] = Payload.checksum[0];
	Mutated.checksum[1] = Payload.checksum[1];
	cout << endl << "when Payload is mutated:";
	cout << endl<<"result of errorchecking: "<<errorchecking(Mutated);
	Mutated = checksum(Mutated);
	cout <<"checksum bytes of mutated Net pkt: ";
	for(uint8_t i=0;i<2;i++)
	{
		cout << hex <<(int)Mutated.checksum[i] << " " ;
	}
	return 0;
}

// void uart_input(char 
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
// 		//tx_msg(uart_buff);
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
// 	// uart_input(UDR0);
// 	_delay_ms(0);
// }
