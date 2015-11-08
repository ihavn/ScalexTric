/*! @file serial.c */

/* ################################################## Standard includes ################################################# */
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
/* ################################################### Project includes ################################################# */
#include "serial.h"

/* ################################################### Global Variables ################################################# */
/* ############################################ Module Variables/Declarations ########################################### */
#define serBAUD_DIV_CONSTANT			( ( unsigned long ) 16 )

/* Constants for writing to UCSRB. */
#define serRX_INT_ENABLE				( ( unsigned char ) 0x80 )
#define serRX_ENABLE					( ( unsigned char ) 0x10 )
#define serTX_ENABLE					( ( unsigned char ) 0x08 )
#define serTX_INT_ENABLE				( ( unsigned char ) 0x20 )

/* Constants for writing to UCSRC. */
#define serUCSRC_SELECT					( ( unsigned char ) 0x80 )
#define serEIGHT_DATA_BITS				( ( unsigned char ) 0x06 )


#if defined (__AVR_ATmega2560__)
volatile uint8_t * _com_port_2_udr[] = {&UDR0, &UDR1, &UDR2, &UDR3};
#elif defined (__AVR_ATmega2561__)
volatile uint8_t * _com_port_2_udr[] = {&UDR0, &UDR1};	
#endif

// Instance data
struct serial_struct{
	// 	volatile uint8_t *_cs_port;
	// 	uint8_t _cs_pin;
	// 	uint8_t _cs_active_level;
	// 	uint8_t _SPCR;
	// 	uint8_t _SPSR;
	
	
	volatile uint8_t *ser_UDR;
		
	buffer_struct_t *_tx_buf;
	buffer_struct_t *_rx_buf;

	void(*_call_back )(serial_p,uint8_t);
};

/*-----------------------------------------------------------*/
#define SERIAL_TX_INTERRUPT_ON()							\
{															\
	unsigned char ucByte;									\
	ucByte = UCSR0B;										\
	ucByte |= serTX_INT_ENABLE;								\
	UCSR0B = ucByte;										\
}

/*-----------------------------------------------------------*/
#define SERIAL_TX_INTERRUPT_OFF()							\
{															\
	unsigned char ucInByte;									\
	ucInByte = UCSR0B;										\
	ucInByte &= ~serTX_INT_ENABLE;							\
	UCSR0B = ucInByte;										\
}

/*-----------------------------------------------------------*/
serial_p serial_new_instance(e_com_port_t com_port, e_baud_t baud, e_data_bit_t data_bit, e_stop_bit_t stop_bit, e_parity_t parity, buffer_struct_t *rx_buf, buffer_struct_t *tx_buf, void(*handler_call_back )(serial_p, uint8_t)) {
	serial_p _serial = malloc(sizeof *_serial);
	
	switch (com_port) {		
		case ser_USART0:
		{
			_serial->ser_UDR = &UDR0;
			break;
		}
	}
	_serial->_tx_buf = tx_buf;
	_serial->_rx_buf = rx_buf;
	
	_serial->_call_back = handler_call_back;
	
	return _serial;
}

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
	unsigned long ulBaudRateCounter;
	unsigned char ucByte;

	portENTER_CRITICAL();
	{
		/* Create the queues used by the com test task. */
		xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
		xCharsForTx = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

		/* Calculate the baud rate register value from the equation in the
		data sheet. */
		ulBaudRateCounter = ( configCPU_CLOCK_HZ / ( serBAUD_DIV_CONSTANT * ulWantedBaud ) ) - ( unsigned long ) 1;

		/* Set the baud rate. */
		UBRR0 = ulBaudRateCounter;

		/* Enable the Rx interrupt.  The Tx interrupt will get enabled
		later. Also enable the Rx and Tx. */
		UCSR0B = ( serRX_INT_ENABLE | serRX_ENABLE | serTX_ENABLE );

		/* Set the data bits to 8. */
		UCSR0C = serEIGHT_DATA_BITS;
	}
	portEXIT_CRITICAL();
	
	/* Unlike other ports, this serial code does not allow for more than one
	com port.  We therefore don't return a pointer to a port structure and can
	instead just return NULL. */
	return NULL;
}

/*-----------------------------------------------------------*/
signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* Only one port is supported. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

/*-----------------------------------------------------------*/
signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	/* Only one port is supported. */
	( void ) pxPort;

	/* Return false if after the block time there is no room on the Tx queue. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	vInterruptOn();

	return pdPASS;
}

/*-----------------------------------------------------------*/
void vSerialClose( xComPortHandle xPort )
{
	unsigned char ucByte;

	/* The parameter is not used. */
	( void ) xPort;

	/* Turn off the interrupts.  We may also want to delete the queues and/or
	re-install the original ISR. */

	portENTER_CRITICAL();
	{
		vInterruptOff();
		ucByte = UCSR0B;
		ucByte &= ~serRX_INT_ENABLE;
		UCSR0B = ucByte;
	}
	portEXIT_CRITICAL();
}

/*-----------------------------------------------------------*/
ISR(USART0_RX_vect)
{
	signed char cChar;
	signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */
	cChar = UDR0;

	xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		taskYIELD();
	}
}

/*-----------------------------------------------------------*/
ISR(USART0_UDRE_vect)
{
	signed char cChar, cTaskWoken;

	if( xQueueReceiveFromISR( xCharsForTx, &cChar, &cTaskWoken ) == pdTRUE )
	{
		/* Send the next character queued for Tx. */
		UDR0 = cChar;
	}
	else
	{
		/* Queue empty, nothing to send. */
		vInterruptOff();
	}
}

