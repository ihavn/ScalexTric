/*
 * main.c
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
#include "marshal.h"

#define startup_TASK_PRIORITY               ( tskIDLE_PRIORITY     )
#define lap_counter_TASK_PRIORITY           ( tskIDLE_PRIORITY + 1 )

#define DEBUG 1

static const uint8_t BT_RX_QUEUE_LENGTH = 30;

static SemaphoreHandle_t sem_goal_line            = NULL;
static QueueHandle_t     xBT_received_chars_queue = NULL;
static SemaphoreHandle_t mu_lap_num               = NULL;
static uint16_t          lap_num                  = 0;

static void inc_lap_num() {
	xSemaphoreTake(mu_lap_num, portMAX_DELAY);
	lap_num++;
	xSemaphoreGive(mu_lap_num);
}

static uint16_t get_lap_num() {
	uint16_t n;
	xSemaphoreTake(mu_lap_num, portMAX_DELAY);
	n = lap_num;
	xSemaphoreGive(mu_lap_num);
	return n;
}

static void reset_lap_num() {
	xSemaphoreTake(mu_lap_num, portMAX_DELAY);
	lap_num = 0;
	xSemaphoreGive(mu_lap_num);
}

static void drive(uint8_t speed) {
	set_motor_speed(speed);
	set_brake_light(0);
	set_head_light(1);
}

static void brake(uint8_t brake) {
	set_brake(brake);
	set_brake_light(1);
	set_head_light(0);
}

#pragma region FSM

#define BT_CMD_RESET     0b11111111

#define CMD_LEARN        0b0001
#define CMD_PROGRAM      0b0010
#define CMD_RUN          0b0011
#define CMD_M_CTRL       0b0100

#define SUB_PROGRAM_STEP 0b0010
#define SUB_PROGRAM_STOP 0b1111

#define MAX_INSTRUCTIONS 1024

typedef enum {
    IDLE,
    M_CTRL,
    LEARN,
    PROGRAMMABLE,
    PROGRAM,
    PROGRAM_STEP,
    RUNNABLE,
} State;

typedef struct {
	uint16_t tacho;
	int8_t   speed;
} Entry;

static Entry instructions[MAX_INSTRUCTIONS];
static uint16_t pc;
static uint8_t  dc;

static State idle_state(uint8_t bt_cmd);
static State m_ctrl_state(uint8_t bt_cmd);
static State learn_state(uint8_t bt_cmd);
static State programmable_state(uint8_t bt_cmd);
static State program_state(uint8_t bt_cmd);
static State program_step_state(uint8_t data);
static State runnable_state(uint8_t bt_cmd);
static State run_state(uint8_t bt_cmd);

void fsm_run(uint8_t bt_cmd) {
	static State state = IDLE;

	if(bt_cmd == BT_CMD_RESET) {
		pc=0;
		dc=0;
		brake(100);
		state = IDLE;
	}

	switch(state) {
	case IDLE:
		state = idle_state(bt_cmd);
		break;
	case M_CTRL:
		state = m_ctrl_state(bt_cmd);
		break;
	case LEARN:
		state = learn_state(bt_cmd);
		break;
	case PROGRAMMABLE:
		state = programmable_state(bt_cmd);
		break;
	case PROGRAM:
		state = program_state(bt_cmd);
		break;
	case PROGRAM_STEP:
		state = program_step_state(bt_cmd);
		break;
	case RUNNABLE:
		state = runnable_state(bt_cmd);
	}
}

State idle_state(uint8_t bt_cmd) {
	uint8_t cmd     = (bt_cmd >> 4) & 0xf;

	switch(cmd) {
	case CMD_M_CTRL:
		return m_ctrl_state(bt_cmd);
	case CMD_LEARN:
		return learn_state(bt_cmd);
	}
	
	return IDLE;
}

State m_ctrl_state(uint8_t bt_cmd) {
	uint8_t cmd     = (bt_cmd >> 4) & 0xf;
	uint8_t sub_cmd = (bt_cmd     ) & 0xf;
	
	switch(cmd) {
	case CMD_M_CTRL:
		if(sub_cmd == 0) {
			brake(100);
			return IDLE;
		}
		uint8_t speed = sub_cmd;
		speed *= 3;
		speed += 55;
        if (speed > 100)
			speed = 100;
        if (speed < 0)
			speed = 0;

		drive(speed);
	}
	
	return M_CTRL;
}

State learn_state(uint8_t bt_cmd) {
	uint8_t n_laps  = (bt_cmd     ) & 0xf;

	reset_lap_num();
	get_tacho_count(); /* discard */
	drive(65);
	uint8_t n;
	uint8_t n_max = 0;
	uint16_t ttl_tacho = 0;
	while((n = get_lap_num()) < n_laps) {
		/* learning_response                                                                      */
		/* -------------------------------------------------------------------------------------- */
		/* | CMD  | N_ROUND | TICKS_T | N_TACHO | X_ACC | Y_ACC | Z_ACC | X_ROT | Y_ROT | Z_ROT | */
		/* -------------------------------------------------------------------------------------- */
		/* | 0001 |    xxxx |     u16 |     u16 |   u16 |   u16 |   u16 |   u16 |   u16 |   u16 | */
		/* -------------------------------------------------------------------------------------- */
		bt_send_u8(0b00010000 | (n&0b00001111));
		
		if(n_max < n) {
			n_max = n;
			ttl_tacho = 0;
		}
		
		ttl_tacho += get_tacho_count();

		bt_send_u16(xTaskGetTickCount());
		bt_send_u16(ttl_tacho);
		bt_send_u16(get_raw_x_accel());
		bt_send_u16(get_raw_y_accel());
		bt_send_u16(get_raw_z_accel());
		bt_send_u16(get_raw_x_rotation());
		bt_send_u16(get_raw_y_rotation());
		bt_send_u16(get_raw_z_rotation());
		
		vTaskDelay(20);
	}
	brake(100);

	/* done_learn_response */
	/* ------------------- */
	/* | CMD  |  SUB_CMD | */
	/* ------------------- */
	/* | 0001 |     1111 | */
	/* ------------------- */
	bt_send_u8(0b00011111);

	return PROGRAMMABLE;
}

State programmable_state(uint8_t bt_cmd) {
	uint8_t cmd     = (bt_cmd >> 4) & 0xf;

	switch(cmd) {
	case CMD_PROGRAM:
		pc = 0;
		return program_state(bt_cmd);
	}

	return PROGRAMMABLE;
}

State program_state(uint8_t bt_cmd) {
	uint8_t cmd     = (bt_cmd >> 4) & 0xf;
	uint8_t sub_cmd = (bt_cmd     ) & 0xf;

	/* program_request                              */
	/* -------------------------------------------- */
	/* | CMD  |  SUB_CMD | N_TACHO | TARGET_SPEED | */
	/* -------------------------------------------- */
	/* | 0010 |     0010 |     u16 |        uint8 | */
	/* -------------------------------------------- */
	switch(cmd) {
	case CMD_PROGRAM:
		switch(sub_cmd) {
		case SUB_PROGRAM_STEP:
			/* discard CMD and SUB_CMD and treat */
			/* the following 3 bytes as raw data */
			return PROGRAM_STEP;
		case SUB_PROGRAM_STOP:
			/* send instructions before running  */
#ifdef DEBUG
			for(uint16_t i=0; i<pc; ++i) {
				Entry e = instructions[i];
				bt_send_u16(e.tacho);
				bt_send_u8(e.speed);
			}
#endif
			return RUNNABLE;
		}
	}
	
	return PROGRAM;
}

State program_step_state(uint8_t data) {
	static uint8_t buf[3];

	buf[dc++] = data;
	dc %= 3;

	if(dc == 0) {
		if(pc > MAX_INSTRUCTIONS)
			err_instruction_overflow();
		Entry e;
		e.tacho = (uint16_t) (buf[0]<<8 | buf[1]);
		e.speed = buf[2];
		instructions[pc++] = e;
		return PROGRAM;
	}
	
	return PROGRAM_STEP;
}

State runnable_state(uint8_t bt_cmd) {
	uint8_t cmd     = (bt_cmd >> 4) & 0xf;

	switch(cmd) {
	case CMD_RUN:
		return run_state(bt_cmd);
	}
	
	return RUNNABLE;
}

State run_state(uint8_t bt_cmd) {
	uint8_t n_laps  = (bt_cmd     ) & 0xf;

	if(n_laps == 0)
		return IDLE;
	
	get_tacho_count(); /* discard */
	for(uint8_t lap=0; lap<n_laps; ++lap) {
		uint16_t ttl_tacho = 0;
		for(uint16_t i=0; i<pc; ++i) {
			Entry e = instructions[i];
			while(ttl_tacho < e.tacho) {
				ttl_tacho += get_tacho_count();
			}
			if(e.speed < 0)
				brake((uint8_t) e.speed);
			else
				drive((uint8_t) e.speed);
		}
	}
	brake(100);
	
	return RUNNABLE;
}

#pragma endregion

#pragma region BT_COMM

uint8_t bt_initialised = 0;

void bt_status_callback(uint8_t status) {
	if (status == DIALOG_OK_STOP) {
		bt_initialised = 1;
	} else if (status == DIALOG_ERROR_STOP) {
		err_bt_comm();
	}
}

void bt_cmd_callback(uint8_t cmd) {
	if (!bt_initialised) {
		err_bt_comm();
		return;
	}
	
	fsm_run(cmd);
}

void bt_debug_callback(uint8_t cmd) {
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
		case 'D': { set_motor_speed(60); set_head_light(1);  break; }
		case 'e': { set_brake(0);        set_brake_light(0); break; }
		case 'E': { set_brake(100);      set_brake_light(1); break; }
	}
}

#pragma endregion

#pragma region TASKS

static void vLapCounterTask(void* pvParameters) {
	(void) pvParameters; /* The parameters are not used. */
	
	for(;;) {
		xSemaphoreTake(sem_goal_line, portMAX_DELAY);
		inc_lap_num();
	}
}

static void vMainTask(void* pvParameters) {
	(void) pvParameters; /* The parameters are not used. */
	
	xBT_received_chars_queue = xQueueCreate(BT_RX_QUEUE_LENGTH, (unsigned portBASE_TYPE) sizeof(uint8_t));

	sem_goal_line = xSemaphoreCreateBinary();
	if(sem_goal_line == NULL) {
		/* There was insufficient OpenRTOS heap available */
		/* for the semaphore to be created successfully.  */
		err_mem_alloc();
	} else {
		set_goal_line_semaphore(sem_goal_line);
	}

	mu_lap_num = xSemaphoreCreateMutex();
	if(mu_lap_num == NULL) {
		/* There was insufficient OpenRTOS heap available */
		/* for the semaphore to be created successfully.  */
		err_mem_alloc();
	}
	
	/* Initialize Bluetooth Module */
	vTaskDelay(1000/portTICK_PERIOD_MS);
	set_bt_reset(0); /* Disable reset line of Blue tooth module */
	vTaskDelay(1000/portTICK_PERIOD_MS);
	init_bt_module(bt_status_callback, xBT_received_chars_queue);

	xTaskCreate(vLapCounterTask, "T_LapCnt", configMINIMAL_STACK_SIZE, NULL, lap_counter_TASK_PRIORITY, NULL);

	uint8_t cmd;
	for(;;) {
		xQueueReceive(xBT_received_chars_queue, &cmd, portMAX_DELAY);
		bt_cmd_callback(cmd);
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	err_stack_overflow();
}

int main(void) {
	init_main_board();
	xTaskCreate(vMainTask, "T_Main", configMINIMAL_STACK_SIZE, NULL, startup_TASK_PRIORITY, NULL);
	vTaskStartScheduler();
}

#pragma endregion
