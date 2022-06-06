// Written by: Travis Lam Han Yuen
// Student ID: 30582105


#include "DLLnPHY.h"

DLL::DLL()
{
	framenum = 0;
	read_until_index_up = 0;
    read_until_index_down = 0;
	Seq = 0;
	Ack = 0;
	CSmethod =0;
}

void DLL::from_NET_layer(uint8_t networkpayload[])
{
    uint16_t length =0; 
    while (NET_packet[length] != '\0')
	{
		length++;
	}
    for(uint8_t i = 0; i<length;i++)
    {
        *NET_queue_down[i] = networkpayload[i];
    }
    addressing(NET_queue_down[2],NET_queue_down[3]);
    // determine the frame number to be send, each frame of DLL has 32 bits.
    framenum = length/32;
    if((length%32)>0)
    {
        framenum+=1;
    }
    return;
}
 
Packet DLL::to_PHY_layer()
{
    return Payload;
}

uint8_t DLL::to_NET_layer()
{
    return NET_queue_up;
}
////////////// on sending ///////////
void DLL::putNET()
{
    for(uint8_t j = 0;j<23;j++)
    {
        Payload.Netpkt[j] = NET_queue_down[read_until_index];
		read_until_index++;
    }
    return;
}

void DLL::putleng()
{
    Payload.Length = (uint8_t) sizeof(Payload.Netpkt);
	return;
}

void DLL::addressing(uint8_t source,uint8_t destination)
{
	uint8_t addressbytes[2] = { 0x00,0x00};
    addressbytes[0] = source;
    addressbytes[1] = destination;
    Payload.Address[0] = addressbytes[0];
	Payload.Address[1] = addressbytes[1];
    return;
}

void DLL::checksum()
{
    uint16_t A[sizeof(Payload.Netpkt)];
    uint16_t B[sizeof(Payload.Netpkt)];
    uint16_t accumulate = 0;
	uint16_t accumulate_of_A = 0;
    for(uint8_t i=0;i<sizeof(Payload.Netpkt);i++)
    {
        accumulate += Payload.Netpkt[0];
        A[i] = accumulate;
        accumulate_of_A += A[i];
        B[i] = accumulate_of_A;
    }
    Payload.checksum[0] = A[sizeof(Payload.Netpkt)-1]%256;
    Payload.checksum[1] = B[sizeof(Payload.Netpkt)-1]%256; //  modulus 256 because 1 byte is 2**8 bits
    return;
}

Packet checksum(Packet Payload)
{
	uint8_t length =23;
	while(Payload.Netpkt[length] != 0)
	{
		length++;
	}
	cout<<dec <<(int)length << endl;
    uint16_t A[length];
    uint16_t B[length];
    uint16_t accumulate = 0;
	uint16_t accumulate_of_A = 0;
    for(uint8_t i=0;i<length;i++)
    {
        accumulate += Payload.Netpkt[i];
        A[i] = accumulate;
        accumulate_of_A += A[i];
        B[i] = accumulate_of_A;
    }
    Payload.checksum[0] = A[length-1]%256;
    Payload.checksum[1] = B[length-1]%256; //  modulus 256 because 1 byte is 2**8 bits
    return Payload;
}
void DLL::controlbytes()
{
    Payload.Control[0] = (Seq<<4) | Ack;
    Payload.Control[1] = (CSmethod<<4) | framenum;
    return;
}

void DLL::encode_everything()
{
/*  Serialize-  1.Header, 
                2.Control,
                3.Addressing,
                4.Length,
                5.Netpkt,
                6.Checksum,
                7.Footer,
*/
    putNET();
    uint16_t byte_count = 0;
    controlbytes();
    putleng();
    checksum();

	Payload.Header = 0x7E;
	Payload.Footer = 0x7E;

    Payload.Everything[byte_count] = Payload.Header;        ///////Header
	byte_count++;

    if(Payload.Control[0]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
        byte_count++;
    }

    Payload.Everything[byte_count] = Payload.Control[0];    /////////Control
	byte_count++;

    if(Payload.Control[1]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.Control[1];
	byte_count++;

    if(Payload.Address[0]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.Address[0];      ///////Address
	byte_count++;

    if(Payload.Address[1]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.Address[1];
	byte_count++;

    if(Payload.Length==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.Length;       /////Length
	byte_count++;

    for(int x=0;x < sizeof(Payload.Netpkt);x++)
    {
        if(Payload.Netpkt[x]==Payload.Header)
        {
            Payload.Everything[byte_count] = flagbyte;
			byte_count++;
        }
        Payload.Everything[byte_count] = Payload.Netpkt[x];    ///// NET
		byte_count++;
    }

    if(Payload.checksum[0]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.checksum[0];      ///// Checksum
	byte_count++;

    if(Payload.checksum[1]==Payload.Header)
    {
        Payload.Everything[byte_count] = flagbyte;
		byte_count++;
    }

    Payload.Everything[byte_count] = Payload.checksum[1];
	byte_count++;

    Payload.Everything[byte_count] = Payload.Footer;          ////// Footer
    return;
}

void DLL::sender()
{
    PHY phy_layer;
    //from_NET_layer(NET_packet);
    while(true)
    {
        from_NET_layer(NET_packet);
        encode_everything();
        phy_layer.from_DLL_layer(Payload);
        Seq++;
        uint8_t latch = Ack;
        while(latch == Ack)
        {
            addr_read();
            _delay_ms(50);
        }
    }
	return;
}

//////////////////// On receiving ////////////////

void DLL::decode_everything(Packet DL_packet_received)
{
    uint16_t byte_count1 = 0;
    uint16_t byte_count2 = 0;
    Payload.Header = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Control[0] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Control[1] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Address[0] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Address[1] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Length = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    for(uint8_t i=0;i<Payload.Length;i++)
    {
        if(DL_packet_received.Everything[byte_count2] == flagbyte)
        {
            byte_count2++;    // see flag then skip
        }
        Payload.Netpkt[i] = DL_packet_received.Everything[byte_count2];
        NET_queue_up[read_until_index] = Payload.Netpkt[i];
        read_until_index_up++;
        framenum--;
        if(framenum==0)
        {
                // send to NET layer 
        }
        Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
        byte_count1++;
        byte_count2++;
    }

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.checksum[0] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.checksum[1] = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    if(DL_packet_received.Everything[byte_count2] == flagbyte)
    {
        byte_count2++;    // see flag then skip
    }

    Payload.Footer = DL_packet_received.Everything[byte_count2];
    Payload.Everything[byte_count1] = DL_packet_received.Everything[byte_count2];
    byte_count1++;
    byte_count2++;

    return;

}

void DLL::addr_read()
{
    PHY phy_layer;
    decode_everything(phy_layer.to_DLL_layer());
    if((Payload.Address[1]>>4) == node_addr)        //   read destination Address
    {
        uint8_t Ackbits = Payload.Control[0]<<4;
        Ackbits = Payload.Control[0]>>4;
        if(errorchecking(Payload))    
        {
        ///  if errorchecking passed, Acknowledge bits +1 and send back to source
            if(Ackbits==15) ///  overflows set back to 0;
            {
                Payload.Control[0] = (Payload.Control[0]>>4);
                Payload.Control[0] = (Payload.Control[0]<<4);  // resets Ack
            }
            else
            {
                Payload.Control[0] += 1;
            }
            addressing(Payload.Address[1],Payload.Address[0]);  // swap the address and send again
            phy_layer.from_DLL_layer(Payload);
            Ack++;
        }
    }
}

bool errorchecking(Packet errorcheck)
{
	uint8_t length =23;
	while(errorcheck.Netpkt[length] != 0)
	{
		length++;
	}
	cout<<dec <<(int)length << endl;
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

/////////////////////////////////// PHY Layer //////////////////////////////////////

Packet PHY::to_DLL_layer()
{
    rx_msg();
    Packet tosend = *message;
    return tosend;
}

void PHY::from_DLL_layer(Packet payload)
{
    *message = Payload;
    tx_msg(message);
    return;
}

void PHY::tx_msg(Packet *msg)
{
	// // Determine the length of the string
	// uint8_t length = 0;
	// while (msg->Everything[length] != '\0')
	// {
	// 	length++;
	// }

	// // Queue message for transmission on rmf12 module
	// rfm12_tx(length, 0xEE, msg->Everything);

	// // Tick the device to transmit
	// rfm12_tick();
}

void PHY::rx_msg()
{
    rx_poll(message);
    return;
}

///////////////////////////// non classified function ////////////////////////
void rx_poll(Packet *on_rx)
{
	// if (rfm12_rx_status() == STATUS_COMPLETE)
	// {
	// 	// Determine the length of the incoming data
	// 	uint8_t rx_length = rfm12_rx_len();
    //     uint8_t *rx[100];

	// 	// Quick sanity check to ensure we are receiving good data
	// 	if ((rx_length == 0) || (rx_length > 100))
	// 	{
	// 		// Malformed data
	// 		return;
	// 	}

	// 	// Receive the data
	// 	memcpy(rx, rfm12_rx_buffer(), rx_length);
    //     bool flag = false;
    //     uint8_t x = 0;
    //     for(uint8_t i = 0; i<sizeof(rx_length); i++)
    //     {
    //         if(*rx[i] == HeaderFooter){flag= !flag;}
    //         while(flag == true)
    //         {
    //             on_rx->Everything[x] = *rx[i];
    //             x++;
    //             if((*rx[i] == HeaderFooter)&&(*rx[i-1]!=flagbyte))
    //             {flag= !flag;}
    //         }
    //     }
	// 	// Clear the chip buffer after we read it
	// 	rfm12_rx_clear();

	// }
}
