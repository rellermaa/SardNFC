/*
 * {main.c}
 *
 * {main application}
 *
 */
#include <NFC.h>

u08_t stand_alone_flag = 1;
u08_t enable = 0;


void main(void)
{
	// WDT ~350ms, ACLK=1.5kHz, interval timer
	WDTCTL = WDT_ADLY_16;
	// Enable WDT interrupt
	IE1 |= WDTIE;
	NFC_Init();
	// Configure UART
	UartSetup();

	McuDelayMillisecond(5);
	UartSendCString("Reader enabled.");
	UartPutCrlf();
	McuDelayMillisecond(2);

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

/********** IT'S A TRAP!!!! (ISR'S) **********/
//===============================================================
#pragma vector= PORT1_VECTOR
__interrupt void PORT1_ISR (void)
{
	while(1);
}

#pragma vector= ADC10_VECTOR
__interrupt void ADC12_ISR (void)
{
	while(1);
}

#pragma vector= USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR (void)
{
	while(1);
}

#pragma vector= USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR (void)
{
	while(1);
}

#pragma vector= TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void)
{
	while(1);
}

#pragma vector= COMPARATORA_VECTOR
__interrupt void COMPARATORA_ISR (void)
{
	while(1);
}

#pragma vector= TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR (void)
{
	while(1);
}

#pragma vector= TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void)
{
	while(1);
}

#pragma vector= NMI_VECTOR
__interrupt void NMI_ISR (void)
{
	while(1);
}
//===============================================================


