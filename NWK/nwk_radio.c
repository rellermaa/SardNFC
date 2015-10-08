/***************************************************************************************************
 *	        Include section					                       		   					       *
 ***************************************************************************************************/
#include "system.h"
#include "radio.h"
#include "uart.h"

#include "nwk_security.h"
#include "nwk_radio.h"

/***************************************************************************************************
 *	        Define section					                       		   					       *
 ***************************************************************************************************/


/***************************************************************************************************
 *	        Prototype section					                       							   *
 ***************************************************************************************************/
uint8 _Modify_Packet_Header(uint8 *packet, uint8 packet_length, uint8 header[]);

/***************************************************************************************************
 *	        Global Variable section  				                            				   *
 ***************************************************************************************************/
uint8 nwk_header[NWK_HEADER_SIZE];

// *************************************************************************************************
// @fn          Radio_Send_Data
// @brief       Send data in NWK layer using wireless radio.
// @param       uint8 *packet			Data packet (payload) that is being sent
//				uint8 len				Length of the data packet
//				uint8 dest_address		Address where the packet is sent
//				uint8 encryption		Encrypt the packet payload
//							PAYLOAD_ENC_OFF		Encrypt the payload
//							PAYLOAD_ENC_OFF		Do not encrypt the payload
//				uint8 ack				Receive ACK after data was sent
//							PCKT_ACK_ON			Ask for ACK
//							PCKT_ACK_OFF		Do not ask for ACK
//				uint8 *error			Save error in case something went wrong
// @return      uint8 EXIT_NO_ERROR		Return with no error
//					  EXIT_ERROR		Return with error
// *************************************************************************************************
uint8 Radio_Send_Data(uint8 *packet, uint8 len, uint8 dest_address, uint8 encryption, uint8 ack, uint8 *error)
{
	// Check if payload is bigger than allowed
	if (len > PAYLOAD_MAX_SIZE) {
		*error = ERR_PAYLOAD_TOO_BIG;
#if (DEBUG_ERR)
	UART_Send_Data("\r\nNWK ERR: Payload too big");
#endif
		return EXIT_ERROR;
	}

	if (encryption) {
		if (len > PAYLOAD_MAX_ENC_SIZE) {
			*error = ERR_PAYLOAD_TOO_BIG;
#if (DEBUG_ERR)
	UART_Send_Data("\r\nNWK ERR: Payload too big");
#endif
			return EXIT_ERROR;
		}
	}

	// If encryption enabled, add one byte to the unencrypted payload
	if (encryption)
		nwk_header[0] = PAYLOAD_ENC_ON;
	else
		nwk_header[0] = PAYLOAD_ENC_OFF;

	// Add security header to the packet and get new length of the packet
	len = _Modify_Packet_Header(packet, len, nwk_header);

	// Modify CTRL byte to ask for ACK
	if (ack) {
		packet[1] |= PKCT_CTRL_ACK_REQ;
	}

	// Encrypt payload
	if (encryption) {
		// Modify CTRL byte to enable security
		//packet[0] |= PKCT_CTRL_SECURITY;
		Payload_Encrypt(packet+1);	// Add +1 to set encryption bit unencrypted
		len = 16+2;		// Set new length because each packet is encrypted with 128 bits = 16 bytes
	}

	// Print out data that was sent
#if (DEBUG_RF)
	uint8 dbg_cntr;
	UART_Send_Data("\r\nNWK Sending:");
	if (encryption)
		UART_Send_Data(" (ENC):");
	for (dbg_cntr = 0; dbg_cntr < len; dbg_cntr++) {
		UART_Send_Byte(packet[dbg_cntr]);
	}
#endif

	// Send packet
	Radio_Tx(packet, len, dest_address, error);
	// DO NOT PUT ANYTHING THAT CAUSE DELAY FROM HERE TO ACK RECEIVING!!!

	// Receive ACK
	if (ack) {
		uint16 timeout = 100;
		uint8 cntr;

		for (cntr=RF_BUFFER_SIZE-1; cntr > 0; cntr--)
			RxPacket[cntr] = 0;

		// Wait for ACK
		Radio_Rx(RxPacket, &len, timeout, &rssi_rx, error);

		// Decrypt payload
		if (encryption)
			Payload_Decrypt(RxPacket+4);

		// Check if received packet is ACK
		if ((RxPacket[5]) != PCKT_TYPE_ACK) {
			Radio_Set_Mode(RADIO_STANDBY);
#if (DEBUG_ERR)
			UART_Send_Data("\r\nNWK ERR: No ACK received");
#endif
			*error = ERR_NO_ACK;
			return EXIT_ERROR;
		} else {
#if (DEBUG_RF)
		UART_Send_Data("\r\nNWK ACK:");
		for (cntr = 0; cntr < len; ++cntr) {
			UART_Send_Byte(RxPacket[cntr]);
		}
#endif
		}

		// If no ACK then repeat N times and then go to sleep
		Radio_Set_Mode(RADIO_STANDBY);
	}

	return EXIT_NO_ERROR;
}


// *************************************************************************************************
// @fn          Radio_Receive_Data
// @brief       Receive data in NWK layer using wireless radio.
// @param       uint8 *packet			Data packet (payload) that is being sent
//				uint8  length			Length of the data packet
//				uint16 timeout			Timeout in millisecons how long to wait until packet received
//				uint8 *rssi				Get the signal strength during packet receiving
//				uint8 *error			Save error in case something went wrong
// @return      uint8 EXIT_NO_ERROR		Return with no error
//					  EXIT_ERROR		Return with error
// *************************************************************************************************
uint8 Radio_Receive_Data(uint8 *packet, uint8 *length, uint16 timeout, uint8 *rssi, uint8 *error) {
	uint32 timeout_ms = 0;		// Timeout in ms before ACK is sent
	uint32 timeout_ack = SYSTEM_SPEED_MHZ*10*timeout_ms;
	uint8 dest_address;
	uint8 ack_status;		// Get the ACK status from the packet header
	uint8 encryption;		// Get the encryption key from the packet header

	// Receive data
	if(Radio_Rx(packet, length, timeout, rssi, error)) return 1;

	// Get encryption bit
	encryption = packet[3];

	// Decrypt received data
	if (encryption)
		Payload_Decrypt(packet+4);

	// Extract from which source packet was coming from to send it back to the same device
	dest_address = packet[2];

	// Check if we need to send ACK back
	ack_status = packet[4] & PKCT_CTRL_ACK_REQ;

	// If ACK sending enabled
	if (ack_status) {
		// Clean Tx Packet array
		uint8 cntr;
		for (cntr=RF_BUFFER_SIZE-1; cntr > 0; cntr--)
			TxPacket[cntr] = 0;

		packet_len = 0;

		// Prepare packet
		TxPacket[packet_len] = PCKT_CTRL | PKCT_CTRL_ANSWER;

		// Change encryption flag in CONTROL byte
		if (encryption)
			TxPacket[packet_len++] |= PKCT_CTRL_SECURITY;
		else
			packet_len++;

		TxPacket[packet_len++] = PCKT_TYPE_ACK;

		// If encryption enabled, add one byte to the unencrypted payload
		if (encryption)
			nwk_header[0] = PAYLOAD_ENC_ON;
		else
			nwk_header[0] = PAYLOAD_ENC_OFF;

		// Add security byte to the packet header and get new length of the packet
		packet_len = _Modify_Packet_Header(TxPacket, packet_len, nwk_header);

		// Encrypt payload
		if (encryption) {
			packet_len = 16+2;
			Payload_Encrypt(TxPacket+1);
		}

		// Add some delay because another side is not yet ready to receive ACK
		while (timeout_ack--) {
			_NOP();
		}

		// Send ACK
		Radio_Tx(TxPacket, packet_len, dest_address, error);

#if (DEBUG_RF)
		UART_Send_Data("\r\nNWK Sending ACK: ");
		for (cntr = 0; cntr < packet_len; ++cntr) {
			UART_Send_Byte(TxPacket[cntr]);
		}
#endif
	}
	// Switch back to receive mode
	Radio_Set_Mode(RADIO_RX);

	return EXIT_NO_ERROR;
}


// *************************************************************************************************
// @fn          _Modify_Packet_Header
// @brief       Add additional header to the packet
// @param       uint8 *packet			Data packet (payload) that is being sent
//				uint8  packet_length	Size of the *packet (payload)
//				uint8  header[]			Additional header that will be added to the beginning of the packet
// @return      uint8 (new_length)		New length of the packet (payload)
// *************************************************************************************************
uint8 _Modify_Packet_Header(uint8 *packet, uint8 packet_length, uint8 header[]) {
	uint8 var;
	uint8 payload_offset = sizeof(header)-1;		// Number of bytes to add to the packet => size of header
													// For some reason sizeof returns actual size increased by 1
	uint8 preserve_first_bytes = 0;					// Number of bytes in the beginning of packet that are not allowed to move

	for (var=packet_length; var > preserve_first_bytes; var--) {
		packet[var+payload_offset-1] = packet[var-1];
	}
	packet[preserve_first_bytes+0] = header[0];

	return packet_length+payload_offset;
}
