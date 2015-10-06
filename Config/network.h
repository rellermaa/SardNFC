#ifndef CONFIG_NETWORK_H_
#define CONFIG_NETWORK_H_

/***************************************************************************************************
 *	        Include section					                       		   					       *
 ***************************************************************************************************/
#include "general.h"

// Network address
#define NETWORK_ADDR1		0x41	// "A"
#define NETWORK_ADDR2		0x42	// "B"
#define NETWORK_ADDR3		0x43	// "C"
#define NETWORK_ADDR4		0x01	// 0x01

// Node address
#ifdef TRANSMITTER
	#define ADDR_LOCAL		0x01	// Local Node address
	#define ADDR_REMOTE		0x03	// Receiver address
#endif
#ifdef RECEIVER
	#define ADDR_LOCAL		0x03
	#define ADDR_REMOTE		0x01
#endif
#ifdef TRANCEIVER
	#define ADDR_LOCAL		0x02	// Local Node address
#endif

#ifndef TRANSMITTER
#ifndef RECEIVER
#ifndef TRANCEIVER
#error Receiver or transmitter not chosen!
#endif
#endif
#endif


/* RF speed settings. Available options:
 * RF_DATA_RATE_2			Range +6dB
 * RF_DATA_RATE_10			Range -1dB
 * RF_DATA_RATE_33			Range -2dB
 * RF_DATA_RATE_100			Range -5.5dB  */
#define RF_DATA_RATE		RF_DATA_RATE_2


/* TX Power. Available options:
 * TXCON_N8DBM		-8dBm
 * TXCON_N5DBM		-5dBm
 * TXCON_N2DBM		-2dBm
 * TXCON_1DBM		+1dBm
 * TXCON_4DBM		+4dBm
 * TXCON_7DBM		+7dBm
 * TXCON_10DBM		+10dBm
 * TXCON_13DBM		+13dBm */
#define TX_POWER			TXCON_13DBM


/* RF Channel. Available options
 * 1		862 000 kHz
 * 2		862 400 kHz
 * 3		862 800 kHz
 * 4		863 200 kHz
 * 5		863 600 kHz
 * 6		864 000 kHz
 * 7		864 400 kHz
 * 8		864 800 kHz
 * 9		865 200 kHz
 * 10		865 600 kHz
 * 11		866 000 kHz
 * 12		866 400 kHz
 * 13		866 800 kHz
 * 14		867 200 kHz
 * 15		867 600 kHz
 * 16		868 000 kHz
 * 17		868 400 kHz
 * 18		868 800 kHz
 * 19		869 200 kHz
 * 20		869 600 kHz
 * 21		870 000 kHz */
#define RF_CHANNEL		11



// 128bit AES encryption key for payload encryption
//#define ENCRYPTION_KEY	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};



#endif /* CONFIG_NETWORK_H_ */
