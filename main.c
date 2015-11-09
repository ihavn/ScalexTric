/*
* Firmware.c
*
* Created: 21-10-2015 11:48:30
*  Author: IHA
*/

#include "include/board.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

uint8_t bt_initialised = 0;

void bt_status_call_back(uint8_t status) {
	if (status == DIALOG_OK_STOP) {
		bt_initialised = 1;
		} else if (status == DIALOG_ERROR_STOP) {
		// ?????
	}
}

void bt_com_call_back(uint8_t byte) {
	char buf[20];
	
	if (bt_initialised) {
		switch (byte) {
			case 'a': {
				set_head_light(0);
				break;
			}
			
			case 'A': {
				set_head_light(1);
				break;
			}
			
			case 'b': {
				set_brake_light(0);
				break;
			}
			
			case 'B': {
				set_brake_light(1);
				break;
			}
			
			case 'c': {
				set_horn(0);
				break;
			}
			
			case 'C': {
				set_horn(1);
				break;
			}
			
			case 'd': {
				set_motor_speed(0);
				break;
			}
			
			case 'D': {
				set_motor_speed(25);
				break;
			}			
			
			case 'E': {
				uint16_t raw_x = get_raw_x_accel();
				sprintf(buf, "raw-x:%4d", raw_x);
				bt_send_bytes((uint8_t *)buf, strlen(buf));
				break;
			}
			default:;
		}
	}
}

int main(void)
{
	init_main_board();
	
	sei();
	
	// 	set_motor_speed(0);
	// 	set_motor_speed(25);
	// 	set_motor_speed(100);
	// 	set_motor_speed(-1);
	// 	set_motor_speed(-10);

	// 	set_head_light(1);
	// 	set_brake_light(1);
	
	// 	set_horn(1);
	// 	_delay_ms(500);
	// 	set_horn(0);

	set_bt_reset(0);  // Disable reset line of Blue tooth module
	init_bt_module(bt_status_call_back, bt_com_call_back);
	
	while(1)
	{
		_delay_ms(100);
		board_tick_100_ms();
	}
}