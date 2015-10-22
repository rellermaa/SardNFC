/*
 * {main.c}
 *
 * {main application}
 *
 */
#include <NFC.h>
#include "system.h"
#include "network.h"
#include "general.h"

// Drivers
#include "uart.h"
#include "soft_spi.h"
#include "radio.h"

// Network
#include "nwk_security.h"
#include "nwk_radio.h"

u08_t stand_alone_flag = 1;
u08_t enable = 0;


void main(void)
{
	// WDT ~350ms, ACLK=1.5kHz, interval timer
	WDTCTL = WDT_ADLY_16;

	// Configure CPU clock
	System_Set_Speed(SYSTEM_SPEED_MHZ);

	// Enable WDT interrupt
	IE1 |= WDTIE;
	McuDelayMillisecond(2);
	NFC_Init();

	// Ports
	LED_PORT_DIR |=  (LED1);				//Set LED1 pin as output
	LED_PORT_OUT &= ~(LED1);				// Set LED1 pin to LOW
	RF_RESET_OUT();							// Set RF module reset pin as output

	// Configure UART
	UART_Init();		//
	SPI_Init();			// RF module interface

	McuDelayMillisecond(5);
	UART_Send_Data("Reader enabled.\n\r");
	McuDelayMillisecond(2);

	Radio_Init(RF_DATA_RATE, TX_POWER, RF_CHANNEL);	// Initialize RF module with 2kbps speed
	Radio_Set_Channel(RF_CHANNEL);

	// General enable interrupts
	__bis_SR_register(GIE);

	// launchpad LED1
	P1DIR |= BIT0;

	while(1)
	{
		// Enter LPM3
		__bis_SR_register(LPM3_bits);
		if(NFC_Read())
		{
			Print_Card();
		}

	}
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
   //exit LPM3
   __bic_SR_register_on_exit(LPM3_bits);
}



