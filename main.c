/*
 * errors.h
 *
 * Created: 07/12/2015 16:00:13
 *  Author: Peter Skrypalle
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "include/board.h"
#include "FreeRTOS/Source/include/FreeRTOS.h"

#include "task.h"
#include "croutine.h"

#include "errors.h"

#define startup_TASK_PRIORITY				( tskIDLE_PRIORITY )

static const uint8_t           BT_RX_QUEUE_LENGTH       = 30;
static       SemaphoreHandle_t goal_line_semaphore      = NULL;
static       QueueHandle_t     xBT_received_chars_queue = NULL;

uint8_t bt_initialised = 0;

void bt_status_callback(uint8_t status) {
	if (status == DIALOG_OK_STOP) {
		bt_initialised = 1;
	} else if (status == DIALOG_ERROR_STOP) {
		err_bt_comm();
	}
}

void bt_com_callback(uint8_t cmd) {
	if (!bt_initialised) {
		err_bt_comm();
		return;
	}

	switch (cmd) {
		case 'x': { err_mem_alloc();      break; }
		case 'y': { err_bt_comm();        break; }
		case 'z': { err_stack_overflow(); break; }

		case 'a': {	set_head_light(0);    break; }
		case 'A': { set_head_light(1);    break; }
		case 'b': { set_brake_light(0);   break; }
		case 'B': { set_brake_light(1);   break; }
		case 'c': { set_horn(0);          break; }
		case 'C': { set_horn(1);          break; }

		case 'd': { set_motor_speed(0);  set_head_light(0);  break; }
		case 'D': { set_motor_speed(85); set_head_light(1);  break; }
		case 'e': { set_brake(0);        set_brake_light(0); break; }
		case 'E': { set_brake(100);      set_brake_light(1); break; }

		default:;
	}
}

static void vStartupTask(void *pvParameters) {
	/* The parameters are not used. */
	(void) pvParameters;
	
	goal_line_semaphore = xSemaphoreCreateBinary();
	xBT_received_chars_queue = xQueueCreate(BT_RX_QUEUE_LENGTH, (unsigned portBASE_TYPE) sizeof(uint8_t));

	if(goal_line_semaphore == NULL) {
		/* There was insufficient OpenRTOS heap available */
		/* for the semaphore to be created successfully.  */
		err_mem_alloc();
	} else {
		set_goal_line_semaphore(goal_line_semaphore);
	}

	/* Initialize Bluetooth Module */
	vTaskDelay(1000/portTICK_PERIOD_MS);
	set_bt_reset(0); /* Disable reset line of Blue tooth module */
	vTaskDelay(1000/portTICK_PERIOD_MS);
	init_bt_module(bt_status_callback, xBT_received_chars_queue);

	uint8_t cmd;
	for(;;) {
		xQueueReceive(xBT_received_chars_queue, &cmd, portMAX_DELAY);
		bt_com_callback(cmd);
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	err_stack_overflow();
}

int main(void) {
	init_main_board();
	xTaskCreate(vStartupTask, "StartupTask", configMINIMAL_STACK_SIZE, NULL, startup_TASK_PRIORITY, NULL);
	vTaskStartScheduler();
}
