/*! @file board.c */

/* ################################################## Standard includes ################################################# */
#include <avr/io.h>
/* ################################################### Project includes ################################################# */
#include "board_spec.h"
#include "../include/board.h"
/* ################################################### Global Variables ################################################# */
/* ############################################ Module Variables/Declarations ########################################### */
#define MOTOR_CONTROL_PWM_FREQ	25000L
#define MOTOR_CONTROL_PRESCALER	1L
#define MOTOR_CONTROL_TOP		(F_CPU/(MOTOR_CONTROL_PWM_FREQ * MOTOR_CONTROL_PRESCALER)-1L)
/* ################################################# Function prototypes ################################################ */

// ----------------------------------------------------------------------------------------------------------------------
void init_main_board() {
	// HORN
	*(&HORN_PORT_reg + 1) |= _BV(HORN_PIN_bit); // set pin to output

	// HEAD LIGHT
	*(&HEAD_LIGHT_PORT_reg + 1) |= _BV(HEAD_LIGHT_PIN_bit); // set pin to output

	// BRAKE LIGHT
	*(&BRAKE_LIGHT_PORT_reg + 1) |= _BV(BRAKE_LIGHT_PIN_bit); // set pin to output
	
	// MOTOR SPEED Fast-PWM ICR = TOP setup
	// Mode 14
	MOTOR_CONTROL_TCCRA_reg |= _BV(MOTOR_CONTROL_WGM1_bit);
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_WGM2_bit) | _BV(MOTOR_CONTROL_WGM3_bit);
	// Clear OCA on compare match and set on BOTTOM
	MOTOR_CONTROL_OCRA_reg = 0;
	MOTOR_CONTROL_TCCRA_reg |= _BV(MOTOR_CONTROL_COMA1_bit);
	*(&MOTOR_CONTROL_OCRA_reg + 1) |= _BV(MOTOR_CONTROL_OCA_PIN_bit); // set pin to output
	// Clear OCB on compare match and set on BOTTOM
	MOTOR_CONTROL_OCRB_reg = 0;
	MOTOR_CONTROL_TCCRA_reg |= _BV(MOTOR_CONTROL_COMB1_bit);
	*(&MOTOR_CONTROL_OCRB_reg + 1) |= _BV(MOTOR_CONTROL_OCB_PIN_bit); // set pin to output
	
	// PWM Freq set to 25 KHz
	// TOP = F_CPU/(Fpwm*N)-1
	MOTOR_CONTROL_ICR_reg = MOTOR_CONTROL_TOP;
	
	#if (MOTOR_CONTROL_PRESCALER == 1)
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_CS0_bit);    // Prescaler 1 and Start Timer
	#elif ((MOTOR_CONTROL_PRESCALER == 8))
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_CS1_bit);    // Prescaler 8 and Start Timer
	#elif ((MOTOR_CONTROL_PRESCALER == 64))
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_CS0_bit) | _BV(MOTOR_CONTROL_CS1_bit);    // Prescaler 64 and Start Timer
	#elif ((MOTOR_CONTROL_PRESCALER == 256))
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_CS2_bit);    // Prescaler 256 and Start Timer
	#elif ((MOTOR_CONTROL_PRESCALER == 1024))
	MOTOR_CONTROL_TCCRB_reg |= _BV(MOTOR_CONTROL_CS0_bit) | _BV(MOTOR_CONTROL_CS2_bit); ;    // Prescaler 1024 and Start Timer
	#endif
}

// ----------------------------------------------------------------------------------------------------------------------
void set_horn(uint8_t state){
	if (state) {
		HORN_PORT_reg |= _BV(HORN_PIN_bit);
	}
	else {
		HORN_PORT_reg &= ~_BV(HORN_PIN_bit);
	}
}

// ----------------------------------------------------------------------------------------------------------------------
void set_head_light(uint8_t state){
	if (state) {
		HEAD_LIGHT_PORT_reg |= _BV(HEAD_LIGHT_PIN_bit);
	}
	else {
		HEAD_LIGHT_PORT_reg &= ~_BV(HEAD_LIGHT_PIN_bit);
	}
}

// ----------------------------------------------------------------------------------------------------------------------
void set_brake_light(uint8_t state){
	if (state) {
		BRAKE_LIGHT_PORT_reg |= _BV(BRAKE_LIGHT_PIN_bit);
	}
	else {
		BRAKE_LIGHT_PORT_reg &= ~_BV(BRAKE_LIGHT_PIN_bit);
	}
}

// ----------------------------------------------------------------------------------------------------------------------
void set_motor_speed(int8_t speed_percent){
	uint16_t ocr;
	if (speed_percent < -10) {
		speed_percent = -10;
	}
	else if (speed_percent > 100)
	{
		speed_percent = 100;
	}
	
	if (speed_percent < 0) {
		ocr = -speed_percent*MOTOR_CONTROL_TOP/100;
	}
	else {
		ocr = speed_percent*MOTOR_CONTROL_TOP/100;
	}
	// TODO Finish Implementation!!!
}