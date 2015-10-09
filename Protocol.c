#include "Protocol.h"

uint8 StatusVar[4] = {'a','b','c','d'};

void Protocol_Send_Status()
{
	uint8 retries;
	uint8 x;
	uint8 error;
	uint8 length = 5;
	uint8 lokPack[16];

	// Copy data to be sent to a temporary buffer
	for(x = 4;x;x--)
	{
		lokPack[x] = StatusVar[x-1];
		UART_Send_Byte(lokPack[x]);
		UART_Send_Byte(' ');
	}
	lokPack[1] = PROTOCOL_SYS_STATUS;
	Radio_Send_Data(lokPack, length, ADDR_REMOTE, PAYLOAD_ENC_ON, PCKT_ACK_ON, &error);
	if(error == ERR_NO_ACK)
	{
		for(retries = 5;retries;retries--)
		{
			UART_Send_Data("NWK ERR: No ACK received\r\n");
			for(x = 4;x;x--)
				lokPack[x] = StatusVar[x-1];
			lokPack[1] = PROTOCOL_SYS_STATUS;
			if(!(Radio_Send_Data(lokPack, length, ADDR_REMOTE, PAYLOAD_ENC_ON, PCKT_ACK_ON, &error)))
				break;
		}
	}
	else if(error)
	{
		UART_Send_Data("NWK ERR\r\n");

	}
	// Set radio into RX mode
	Radio_Set_Mode(RADIO_RX);
}
