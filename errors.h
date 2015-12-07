/*
 * errors.h
 *
 * Created: 07/12/2015 16:00:13
 *  Author: Peter Skrypalle
 */ 

#ifndef ERRORS_H_
#define ERRORS_H_

#include "FreeRTOS/Source/include/FreeRTOS.h"
#include "task.h"
#include "include/board.h"

void err_mem_alloc();
void err_bt_comm();
void err_stack_overflow();

#endif /* ERRORS_H_ */
