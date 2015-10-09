/*
 * Protocol.h
 *
 *  Created on: 09.10.2015
 *      Author: Rain_2
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

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

#define PROTOCOL_RFID_CARD 		'C'
#define PROTOCOL_SYS_STATUS 	'S'
#define PROTOCOL_STATUS_REQUEST 'R'
#define PROTOCOL_LOCK_REQUEST 	'L'

#define STATUS_RSSI_POS		0
#define STATUS_LOCK_POS		1
#define STATUS_SYS_POS		2

extern uint8 StatusVar[];

void Protocol_Send_Status(void);

#endif /* PROTOCOL_H_ */
