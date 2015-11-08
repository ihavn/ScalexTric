/*
 * board_spec.h
 *
 * Created: 21-10-2015 12:20:18
 *  Author: IHA
 */ 


#ifndef BOARD_SPEC_H_
#define BOARD_SPEC_H_

// CPU
#define F_CPU		16000000L

// SPI
#define SPI_PORT	PORTB
#define SPI_CS_PORT	PORTB
#define SPI_CS		PB0
#define SPI_SCK		PB1
#define SPI_MOSI	PB2
#define SPI_MISO	PB3

// USART

// Bluetooth
#define BT_RTS_PORT				PORTE
#define BT_RTS_PIN				PE2
#define BT_CTS_PORT				PORTE
#define BT_CTS_PIN				PE5
#define BT_RESET_PORT			PORTA
#define BT_RESET_PIN			PA0
#define BT_AUTO_DISCOVERY_PORT	PORTA
#define BT_AUTO_DISCOVERY_PIN	PA1
#define BT_MASTER_PORT			PORTA
#define BT_MASTER_PIN			PA2

// GOAL LINE


// TACHO

// HORN
#define HORN_PORT_reg					PORTC
#define HORN_PIN_bit					PC3

// HEAD LIGHT
#define HEAD_LIGHT_PORT_reg				PORTC
#define HEAD_LIGHT_PIN_bit				PC2

// BRAKE LIGHT
#define BRAKE_LIGHT_PORT_reg			PORTC
#define BRAKE_LIGHT_PIN_bit				PC1

// AUX
#define AUX_PORT_reg					PORTC
#define AUX_PIN_bit						PC0

// MOTOR CONTROL - Timer 3 used
#define MOTOR_CONTROL_TCCRA_reg			TCCR3A
#define MOTOR_CONTROL_TCCRB_reg			TCCR3B
#define MOTOR_CONTROL_TCCRC_reg			TCCR3C
#define MOTOR_CONTROL_COMA0_bit			COM3A0
#define MOTOR_CONTROL_COMA1_bit			COM3A1
#define MOTOR_CONTROL_COMB0_bit			COM3B0
#define MOTOR_CONTROL_COMB1_bit			COM3B1
#define MOTOR_CONTROL_COMC0_bit			COM3C0
#define MOTOR_CONTROL_COMC1_bit			COM3C1
#define MOTOR_CONTROL_WGM0_bit			WGM30
#define MOTOR_CONTROL_WGM1_bit			WGM31
#define MOTOR_CONTROL_WGM2_bit			WGM32
#define MOTOR_CONTROL_WGM3_bit			WGM33
#define MOTOR_CONTROL_CS0_bit			CS30
#define	MOTOR_CONTROL_CS1_bit			CS31
#define MOTOR_CONTROL_CS2_bit			CS32
#define MOTOR_CONTROL_OCRA_reg			OCR3A
#define MOTOR_CONTROL_OCRB_reg			OCR3B
#define MOTOR_CONTROL_OCRC_reg			OCR3C
#define MOTOR_CONTROL_ICR_reg			ICR3
#define MOTOR_CONTROL_TIMSK_reg			TIMSK3
#define MOTOR_CONTROL_TIFR_reg			TIFR3

#define MOTOR_CONTROL_OCA_PORT_reg		PORTE
#define MOTOR_CONTROL_OCA_PIN_bit		PE3
#define MOTOR_CONTROL_OCB_PORT_reg		PORTE
#define MOTOR_CONTROL_OCB_PIN_bit		PE4
#endif /* BOARD_SPEC_H_ */