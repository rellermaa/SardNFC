/*
 * nwk_radio.h
 *
 *  Created on: 20.04.2015
 *      Author: mairo
 */

#ifndef NWK_NWK_RADIO_H_
#define NWK_NWK_RADIO_H_

/***************************************************************************************************
 *	        Include section					                       		   					       *
 ***************************************************************************************************/

/***************************************************************************************************
 *	        Define section					                       		   					       *
 ***************************************************************************************************/
#define PCKT_ACK_ON			1
#define PCKT_ACK_OFF		0

#define NWK_HEADER_SIZE		1
#define ENC_PACKET_SIZE		16

#define PKT_SRC_BYTE		2		// Source byte is third byte in the received packet
#define PKT_ENC_BYTE		3		// Encryption byte is 4th byte in the received packet
#define PKT_CTRL_BYTE		4		// Control byte defines
									// BIT1		PKT_CTRL_RETRY: 	0 - first try; 1 - retry
									// BIT2		Not yet defined
									// BIT3		PKT_CTRL_ACK_REQ: 	0 - no ACK required; 1 - ACK required
									// BIT4		PKT_CTRL_REQUEST	0 - request
									//			PKT_CTRL_ANSWER		1 - answer
									// BIT5-6	PKT_CTRL_HOPS		// Number of allowed hops
									// BIT7-8	Net yet defined
#define PKT_PAYLOAD_BYTE1	5		// First byte of the payload after CTRL byte

#define ACK_WAIT_TIMEOUT	200		// Faster network speed works with 100ms quite well, in case of ERROR, increase timeout value
									// For long packets (up to 64 bytes) a timeout up to 1000ms may be needed

/***************************************************************************************************
 *	        Global Variable section  				                            				   *
 ***************************************************************************************************/
uint8 packet_len;

/*- Types ------------------------------------------------------------------*/
enum
{
  NWK_OPT_ACK_REQUEST          = 1 << 0,
  NWK_OPT_ENABLE_SECURITY      = 1 << 1,
};

typedef struct NWK_DataReq_t
{
  // service fields
  uint8      state;

  // request parameters
  uint8      dstAddr;
  uint8		 localAddr;
  uint8      enc;
  uint8      ack;
  uint8      *data;
  uint8      size;

  // confirmation parameters
  uint8      status;
  uint8      control;
} NWK_DataReq_t;

typedef struct NWK_DataInd_t
{
  uint16     srcAddr;
  uint16     dstAddr;
  uint8      options;
  uint8      *data;
  uint8      size;
  int8       rssi;
} NWK_DataInd_t;

/***************************************************************************************************
 *	        Prototype section					                       							   *
 ***************************************************************************************************/
//uint8 Radio_Send_Data(uint8 *packet, uint8 len, uint8 dest_address, uint8 encryption, uint8 ack, uint8 *error);
uint8 Radio_Send_Data(uint8 *packet, uint8 len, uint8 dest_address, uint8 encryption, uint8 ack);

//uint8 Radio_Receive_Data(uint8 *packet, uint8 *length, uint16 timeout, uint8 *rssi, uint8 *error);
uint8 Radio_Receive_Data(uint8 *packet, uint8 *length, uint16 timeout, uint8 *rssi);

uint8 Radio_Send_Data2(NWK_DataReq_t *packet);


#endif 
