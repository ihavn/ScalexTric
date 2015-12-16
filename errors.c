/*
 * errors.c
 *
 * Created: 07/12/2015 15:58:24
 *  Author: Peter Skrypalle
 */ 

#include "errors.h"

#define ERR(X...) for(;;) { X vTaskDelay(5000); }

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
	/* .... */
	ERR(beep_shrt(); beep_shrt(); beep_shrt(); beep_shrt();)
}

void err_bt_comm() {
	err_notify();
	/* ..._ */
	ERR(beep_shrt(); beep_shrt(); beep_shrt(); beep_long();)
}

void err_stack_overflow() {
	err_notify();
	/* .._. */
	ERR(beep_shrt(); beep_shrt(); beep_long(); beep_shrt();)
}

void err_instruction_overflow() {
	err_notify();
	/* ..__ */
	ERR(beep_shrt(); beep_shrt(); beep_long(); beep_long();)
}