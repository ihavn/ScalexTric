/*
 * errors.c
 *
 * Created: 07/12/2015 15:58:24
 *  Author: Peter Skrypalle
 */ 

#include "errors.h"

static void beep(uint16_t len) {
	set_horn(1);
	vTaskDelay((TickType_t) len);
	set_horn(0);
	vTaskDelay(100);
}

static void beep_shrt()  { beep(200); }
static void beep_long()  { beep(500); }
static void err_notify() { beep(500); vTaskDelay(500); }

void err_mem_alloc() {
	err_notify();
	beep_shrt(); beep_shrt(); beep_shrt(); beep_shrt();
}

void err_bt_comm() {
	err_notify();
	beep_long(); beep_shrt(); beep_shrt(); beep_shrt();
}

void err_stack_overflow() {
	err_notify();
	beep_shrt(); beep_long(); beep_shrt(); beep_shrt();
}