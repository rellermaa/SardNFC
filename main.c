#include <msp430.h> 
#include "system.h"
#include "network.h"
#include "general.h"

// Drivers
#include "uart.h"
#include "spi.h"
#include "radio.h"

// Network
#include "nwk_security.h"
#include "nwk_radio.h"

/***************************************************************************************************
 *	        Global Variable section  				                            				   *
 ***************************************************************************************************/
uint8 error;
uint8 payload_length;
uint8 cntr;

/*
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    error = 0;

	System_Init(&error);
	__enable_interrupt();

#ifdef TRANSMITTER
	UART_Send_Data("TRANSMITTER\r\n");
#endif
#ifdef RECEIVER
	UART_Send_Data("RECEIVER\r\n");
#endif
#ifdef TRANCEIVER
	UART_Send_Data("TRANCEIVER\r\n");
#endif

	while (1)
	{
#ifdef TRANCEIVER
		payload_length = 0;

		// Clear packet
		uint8 cntr;
		for (cntr=RF_BUFFER_SIZE; cntr > 0; cntr--)
			TxPacket[cntr] = 0;

		// Construct the packet
		TxPacket[payload_length++] = PCKT_CTRL | PKCT_CTRL_REQUEST;
		TxPacket[payload_length++] = PCKT_TYPE_VOLTAGE;
		TxPacket[payload_length++] = '-';
		TxPacket[payload_length++] = 't';
		TxPacket[payload_length++] = 'e';
		TxPacket[payload_length++] = 'r';
		TxPacket[payload_length++] = 'e';
		TxPacket[payload_length++] = ' ';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = 'a';
		TxPacket[payload_length++] = 'r';
		TxPacket[payload_length++] = 'd';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = 'y';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = '!';

		// Watch which data is being sent (not encrypted)
		UART_Send_Data("\r\nDBG Sending:");
		for (cntr = 0; cntr < payload_length; cntr++)
		{
			UART_Send_Byte(TxPacket[cntr]);
		}

		/* NWK level data sending */
		// PAYLOAD_ENC_ON - encrypt the payload. If you are using device that has RAM 512bytes (ex. MSP430G2553)
		// and you have added additional code (ADC, external sensors etc) then buffer overflow may appear
		// and data that will be sent is corrupted. To check if data is OK then enable DEBUG mode and check the result of
		// data that is being sent to the Radio. If the data is every time different, then you have a lack of RAM and
		// should disable encryption with PAYLOAD_ENC_OFF
		Radio_Send_Data(TxPacket, payload_length, 1, PAYLOAD_ENC_ON, PCKT_ACK_OFF, &error);

		// Set radio into RX mode
		Radio_Set_Mode(RADIO_RX);

		uint8 len;
		uint8 rssi_env;
		uint8 rssi_rx;
		uint8 var;

		// Clear RX buffer
		for (var = RF_BUFFER_SIZE-1; var > 0; var--)
				RxPacket[var] = 0;

		_BIS_SR(LPM0_bits + GIE);

		// Receive data, wait until 500ms until timeout and continue with other tasks
		//if(RF_IRQ_PORT_IN & RF_IRQ0_PIN)
		//{
			if (Radio_Receive_Data(RxPacket, &len, 500, &rssi_rx, &error))
			{
				// If error
				if (error == ERR_RX_WRONG_LENGTH)
				{
					UART_Send_Data("DBG RF ReInit!\r\n");
					Radio_Init(RF_DATA_RATE, TX_POWER, RF_CHANNEL, &error);
				}
			}
			else
			{// No error
				LED1_TOGGLE();		// Toggle LED1

				// See the unencrypted data that was received
				UART_Send_Data("\r\nDBG Receiving DEC:");
				for (var = 0; var < len; ++var)
					UART0_Send_ByteToChar(&(RxPacket[var]));

				// Get environemnt RSSI value (noise level)
				rssi_env = Radio_Get_RSSI();

				// Print the RSSI values (NOISE LEVEL | RECEIVED_SIGNAL_SRENGTH)
				UART_Send_Data("\r\nDBG RSSI env | rx: ");
				UART0_Send_ByteToChar(&(rssi_env));
				UART_Send_Data(" ");
				UART0_Send_ByteToChar(&(rssi_rx));
			}
		//}

#endif	/* TRANCEIVER */


/* TX part */
#ifdef TRANSMITTER
		payload_length = 0;

		// Clear packet
		uint8 cntr;
		for (cntr=RF_BUFFER_SIZE; cntr > 0; cntr--)
			TxPacket[cntr] = 0;

		// Construct the packet
		TxPacket[payload_length++] = PCKT_CTRL | PKCT_CTRL_REQUEST;
		TxPacket[payload_length++] = PCKT_TYPE_VOLTAGE;
		TxPacket[payload_length++] = '-';
		TxPacket[payload_length++] = 't';
		TxPacket[payload_length++] = 'e';
		TxPacket[payload_length++] = 'r';
		TxPacket[payload_length++] = 'e';
		TxPacket[payload_length++] = ' ';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = 'a';
		TxPacket[payload_length++] = 'r';
		TxPacket[payload_length++] = 'd';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = 'y';
		TxPacket[payload_length++] = 's';
		TxPacket[payload_length++] = '!';


		// Watch which data is being sent (not encrypted)
		UART_Send_Data("\r\nDBG Sending:");
		for (cntr = 0; cntr < payload_length; cntr++)
		{
			UART_Send_Byte(TxPacket[cntr]);
		}

		/* NWK level data sending */
		// PAYLOAD_ENC_ON - encrypt the payload. If you are using device that has RAM 512bytes (ex. MSP430G2553)
		// and you have added additional code (ADC, external sensors etc) then buffer overflow may appear
		// and data that will be sent is corrupted. To check if data is OK then enable DEBUG mode and check the result of
		// data that is being sent to the Radio. If the data is every time different, then you have a lack of RAM and
		// should disable encryption with PAYLOAD_ENC_OFF
		Radio_Send_Data(TxPacket, payload_length, ADDR_REMOTE, PAYLOAD_ENC_ON, PCKT_ACK_ON, &error);

		// Add some delay (around 2sec)
		__delay_cycles(2000000*SYSTEM_SPEED_MHZ);
#endif	/* END: TRANSMITTER */

#ifdef RECEIVER
		uint8 len;
		uint8 rssi_env;
		uint8 rssi_rx;
		uint8 var;

		for (var = RF_BUFFER_SIZE-1; var > 0; var--)
				RxPacket[var] = 0;

		// Set radio into RX mode
		Radio_Set_Mode(RADIO_RX);

		// Enter LPM0 w/ interrupts enabled
		_BIS_SR(LPM0_bits + GIE);

		// Receive data, wait until 500ms until timeout and continue with other tasks
		if (Radio_Receive_Data(RxPacket, &len, 500, &rssi_rx, &error)) {
			// If error
			if (error == ERR_RX_WRONG_LENGTH) {
#ifdef DEBUG
				UART_Send_Data("DBG RF ReInit!\r\n");
#endif /* DEBUG */
				Radio_Init(RF_DATA_RATE, TX_POWER, RF_CHANNEL, &error);
			}
		} else {// No error
			LED1_TOGGLE();		// Toggle LED1


			// See the unencrypted data that was received
			UART_Send_Data("\r\nDBG Receiving DEC:");
			for (var = 0; var < len; ++var) {
				UART0_Send_ByteToChar(&(RxPacket[var]));
			}

			// Get environemnt RSSI value (noise level)
			rssi_env = Radio_Get_RSSI();
#ifdef DEBUG
			// Print the RSSI values (NOISE LEVEL | RECEIVED_SIGNAL_SRENGTH)
			UART_Send_Data("\r\nDBG RSSI env | rx: ");
			UART0_Send_ByteToChar(&(rssi_env));
			UART_Send_Data(" ");
			UART0_Send_ByteToChar(&(rssi_rx));
#endif
		}
#endif	/* RECEIVER */
	}
}
