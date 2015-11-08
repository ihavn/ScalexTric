/*! @file serial.h
@brief Serial driver for the USART0.

@author iha

@defgroup  serial_drivers Driver for ATMEGA52561 USART0.
@{
@brief A driver for USART0.

@note Only implemented for USART0!!!!

@}
*/
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#include "../buffer/buffer.h"

// Abstract Data Type (ADT)
typedef struct serial_struct *serial_p;

typedef enum
{
	ser_USART0,
	ser_USART1,
	ser_USART2,
	ser_USART3
} e_com_port_t;

typedef enum
{
	ser_NO_PARITY,
	ser_ODD_PARITY,
	ser_EVEN_PARITY,
	ser_MARK_PARITY,
	ser_SPACE_PARITY
} e_parity_t;

typedef enum
{
	ser_STOP_1,
	ser_STOP_2
} e_stop_bit_t;

typedef enum
{
	ser_BITS_5,
	ser_BITS_6,
	ser_BITS_7,
	ser_BITS_8
} e_data_bit_t;

typedef enum
{
	ser_50,
	ser_75,
	ser_110,
	ser_134,
	ser_150,
	ser_200,
	ser_300,
	ser_600,
	ser_1200,
	ser_1800,
	ser_2400,
	ser_4800,
	ser_9600,
	ser_19200,
	ser_38400,
	ser_57600,
	ser_115200
} e_baud_t;

serial_p serial_new_instance(e_com_port_t com_port, e_baud_t baud, e_data_bit_t data_bit, e_stop_bit_t stop_bit, e_parity_t parity, buffer_struct_t *rx_buf, buffer_struct_t *tx_buf, void(*handler_call_back )(serial_p, uint8_t));
uint8_t serial_send_buffer( serial_p handle, const signed char  const *str, uint8_t len);
uint8_t serial_send_char( serial_p handle, const char ch);

#endif

