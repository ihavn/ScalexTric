/*
 * Firmware.c
 *
 * Created: 21-10-2015 11:48:30
 *  Author: IHA
 */ 

#include "include/board.h"

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	init_main_board();
	
	sei();
	
	set_motor_speed(0);
 	set_motor_speed(25);
	set_motor_speed(100);
	set_motor_speed(-1);
	set_motor_speed(-10);
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}