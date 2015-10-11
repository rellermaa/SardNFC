//===============================================================
// Program with hardware USART and SPI communication	        ;
// interface with TRF7970A reader chip.                         ;
//                                                              ;
// PORT1.0 - HEART BEAT LED                                     ;
// PORT1.1 - UART RX                                            ;
// PORT1.2 - UART TX                                            ;
// PORT1.5 - SPI DATACLK                                        ;
// PORT1.6 - SPI MISO (REMOVE LED2 JUMPER)                      ;
// PORT1.7 - SPI MOSI                                           ;
//                                                              ;
// PORT2.0 - IRQ (INTERUPT REQUEST from TRF7970A)               ;
// PORT2.1 - SLAVE SELECT                                       ;
// PORT2.2 - TRF7970A ENABLE                                    ;
// PORT2.3 - ISO14443B LED                                      ;
// PORT2.4 - ISO14443A LED                                      ;
// PORT2.5 - ISO15693  LED                                      ;
//===============================================================

/********** HEADER FILES **********/
//===============================================================
#include <NFC.h>
/********** GLOBAL VARIABLES **********/
//===============================================================
u08_t buf[100];					// TX/RX BUFFER FOR TRF7970A
u08_t i_reg = 0x01;             // INTERRUPT REGISTER
u08_t irq_flag = 0x00;
u08_t rx_error_flag = 0x00;
s08_t rxtx_state = 1;           // USED FOR TRANSMIT RECEIVE BYTE COUNT
u08_t host_control_flag = 0;

//===============================================================


void NFC_Init(void)
{
    SLAVE_SELECT_PORT_SET;
	SLAVE_SELECT_HIGH;

	ENABLE_PORT_SET;
	ENABLE_TRF;

	// wait until TRF7970A system clock started
	McuDelayMillisecond(2);

	// settings for communication with TRF7970A
	Trf7970CommunicationSetup();

	// Set Clock Frequency and Modulation
	Trf7970InitialSettings();

	// set the DCO to 8 MHz
	McuOscSel(1);

	// Re-configure the USART with this external clock
	Trf7970ReConfig();

}
