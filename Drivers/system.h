#ifndef SYSTEM_H_
#define SYSTEM_H_

/***************************************************************************************************
 *	        Include section					                       		   					       *
 ***************************************************************************************************/
#include <msp430.h>
#include <stdlib.h>
#include "gpio.h"
#include "network.h"

/***************************************************************************************************
 *	        Define section					                       		   					       *
 ***************************************************************************************************/


/***** ERROR CODES ********/
#define EXIT_NO_ERROR			0
#define EXIT_ERROR				1
#define EXIT_TIMEOUT			2

// RF ERROR CODES
#define ERR_RX_TIMEOUT			11
#define ERR_TX_TIMEOUT			12
#define	ERR_ECC_DECODE			13
#define ERR_RX_WRONG_LENGTH		14
#define ERR_RF_FREQ_FIX_TIMEOUT	15
#define ERR_OUT_OF_MEMORY		16
#define ERR_RX_DATA_NOT_MATCH	17
#define ERR_NO_ACK				20
#define ERR_PAYLOAD_TOO_BIG		21

/**** PACKET DATA TYPES ***/
#define PCKT_TYPE_ACK			0xFF
#define PCKT_TYPE_VOLTAGE		0x01
#define PKCT_TYPE_TEMPERATURE	0x02
#define PKCT_TYPE_HUMIDITY		0x03
/* ADD HERE NEW TYPES IF NEEDED */

/***** PACKET CONTROL ****/
#define PCKT_CTRL				0			// Control byte type
#define PKCT_CTRL_RETRY			(1<<0)		// 0 - first try; 1 - retry
#define PKCT_CTRL_SECURITY		(1<<1)		// 0 - security off; 1 - security on
#define PKCT_CTRL_ACK_REQ		(1<<2)		// 0 - no ACK required; 1 - ACK required
#define PKCT_CTRL_REQUEST		(0<<3)		// 0 - request; 1 - answer
#define PKCT_CTRL_ANSWER		(1<<3)		// 0 - request; 1 - answer
#define PKCT_CTRL_HOPS			(2<<4)		// Number of allowed hops
#define PKCT_NOT_DEFINED		(1<<6)		// Not defined yet
#define PKCT_NOT_DEFINED2		(1<<7)		// Not defined yet


// LED definitions
#define LED1_TOGGLE()			{ LED_PORT_OUT ^= LED1; }
#define LED1_ON()				{ LED_PORT_OUT |= LED1; }
#define LED1_OFF()				{ LED_PORT_OUT &= ~LED1; }

// System speed settings
#define SYSTEM_SPEED_1MHZ		1
#define SYSTEM_SPEED_8MHZ		8
#define SYSTEM_SPEED_12MHZ		12
#define SYSTEM_SPEED_16MHZ		16


/***************************************************************************************************
 *	        Global Variable section  				                            				   *
 ***************************************************************************************************/
typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

typedef signed long    int32;
typedef signed short   int16;
typedef signed char    int8;


/***************************************************************************************************
 *	        Prototype section					                       							   *
 ***************************************************************************************************/
uint8 System_Init(uint8 *error);
void System_Set_Speed(uint8 mhz);

int cByteToHex(char input);
void Blink_LED(uint8 led);
uint8 Delay_Millis(uint16 ms);

#endif /* SYSTEM_H_ */
