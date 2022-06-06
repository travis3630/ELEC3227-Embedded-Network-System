// Written by: Travis Lam Han Yuen
// Student ID: 30582105


//#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>

#define NODE1 1
#define NODE2 5
#define NODE3 15
//#include the library for the RFM12 module and the UART
//#include "rfm12.h"

uint8_t node_addr = NODE1;
uint8_t HeaderFooter = 0x7E;
uint8_t flagbyte = 0x55;
uint8_t* NET_packet[] = {
    0x74, 0x64,
    0x01, 0x0F,   // src and dest addr
    0x79,         // length
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 
    0x5A, // Trans segment x121
    0x7E, 0x4C
};      // an example of NET packet


typedef struct Packet{
    uint8_t Header;  // 1 byte
    uint8_t Footer;  // 1 byte
    uint8_t* Netpkt[];       // 1-23bytes
    uint8_t* checksum[2];    // 2 bytes
    uint8_t Length;         // 1 byte
    uint8_t* Address[2];     // 2 bytes
    uint8_t* Control[2];     // 2 bytes
    uint8_t Everything[];   // bytes
} Packet;

class DLL {       // The class
    public:       // Access specifier
        DLL();    //constructor
        void from_NET_layer(uint8_t* networkpayload[]);
        Packet to_PHY_layer();
        uint8_t to_NET_layer();
    private:
        Packet Payload;
        //void from_NET_layer(uint8_t* networkpayload[]);
        Packet to_PHY_layer();
		uint8_t NET_queue_down[];
        uint8_t NET_queue_up[];
        uint16_t framenum;
		uint16_t read_until_index_down;
        uint16_t read_until_index_up;
		uint8_t Seq;
		uint8_t Ack;
		uint8_t CSmethod;

        /// on sending
        void putNET();
        void putleng();
        void addressing(uint8_t source,uint8_t destination);
        void checksum();
        void controlbytes();
        void sender();
        void encode_everything();

        /// on receiving
        bool errorchecking(Packet errorcheck);
        void addr_read();
        void decode_everything(Packet DL_packet_received);
};

class PHY {
	public:
		void from_DLL_layer(Packet payload);
		Packet to_DLL_layer();
	private:
		Packet* message;
		Packet Payload;
		void tx_msg(Packet* msg);
		void rx_msg();
};

void rx_poll(Packet* on_rx);
