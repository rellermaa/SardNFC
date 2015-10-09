#include <msp430.h> 
#include <NFC.h>
/*
 * main.c
 */


u08_t Tag_Count;

int main(void) {


    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	NFC_Init();
	
    while(1)
    {
		Tag_Count = 0;
		IRQ_OFF;
		DISABLE_TRF;

		// Enter LPM3
		__bis_SR_register(LPM3_bits);

		// launchpad LED1 - Toggle (heartbeat)
		P1OUT ^= BIT0;

		// Clear IRQ Flags before enabling TRF7970A
		IRQ_CLR;
		IRQ_ON;

		ENABLE_TRF;

		// Must wait at least 4.8 ms to allow TRF7970A to initialize.
		__delay_cycles(40000);


		Iso14443aFindTag();	// Scan for 14443A tags

		// Write total number of tags read to UART
		if(Tag_Count > 0){
			Tag_Count = UartNibble2Ascii(Tag_Count & 0x0F);	// convert to ASCII
			UartSendCString("Tags Found: ");
			UartPutChar(Tag_Count);
			UartPutCrlf();
			UartPutCrlf();
		}
    }
}
