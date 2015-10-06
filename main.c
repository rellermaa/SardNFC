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

/*
 * main.c
 */
int main(void)
{
	uint8 error = 0;
	uint8 payload_length = 0;
	uint8 send_data = 0;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	System_Init(&error);
	__enable_interrupt();

	UART_Send_Data("TRANCEIVER\r\n");

	while (1)
	{
		uint8 len = 0;
		uint8 rssi_env = 0;
		uint8 rssi_rx = 0;
		uint8 var = 0;

		// Check if we have data in UART input buffer
		while(uart_buf_size())
		{
			// Print input buffer to terminal for debugging
			uint8 character = uart_read();
			UART_Send_Byte(character);
			// If we get CR, send packet, otherwise append data to radio packet
			if(character == '\r') send_data = 1;
			else TxPacket[payload_length++] = character;
		}

		// If we have packet, send to radio
		if(send_data)
		{
			send_data = 0;

			Radio_Send_Data(TxPacket, payload_length, ADDR_REMOTE, PAYLOAD_ENC_ON, PCKT_ACK_ON, &error);

			payload_length = 0;
			// Set radio into RX mode
			Radio_Set_Mode(RADIO_RX);
		}

		// Receive data, wait until 500ms until timeout and continue with other tasks
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
			UART_Send_Data("\r\nGot packet");
			UART_Send_Data("\r\nLength:");
			UART0_Send_ByteToChar(&(RxPacket[0]));
			UART_Send_Data("\r\nTo:");
			UART0_Send_ByteToChar(&(RxPacket[1]));
			UART_Send_Data("\r\nFrom:");
			UART0_Send_ByteToChar(&(RxPacket[2]));
			UART_Send_Data("\r\nMessage:");
			for (var = 3; var < len; ++var)
			{
				UART_Send_Byte(RxPacket[var]);
			}

			// Get environemnt RSSI value (noise level)
			rssi_env = Radio_Get_RSSI();

			// Print the RSSI values (NOISE LEVEL | RECEIVED_SIGNAL_SRENGTH)
			UART_Send_Data("\r\nDBG RSSI env | rx: ");
			UART0_Send_ByteToChar(&(rssi_env));
			UART_Send_Data(" ");
			UART0_Send_ByteToChar(&(rssi_rx));
		}
	}
}
