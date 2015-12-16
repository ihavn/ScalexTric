/*
 * marshal.c
 *
 * Created: 08/12/2015 15:41:05
 *  Author: Peter Skrypalle
 */ 

#include "marshal.h"

static void bt_blocking_send(uint8_t* bytes, uint8_t len) {
	uint8_t status;
	do {
		status = bt_send_bytes(bytes, len);
	} while(status == BUFFER_FULL);
}

void bt_send_u8(uint8_t u8) {
	bt_blocking_send(&u8, sizeof(uint8_t));
}

void bt_send_u16(uint16_t u16) {
	uint8_t buf[2];
	buf[0] = (u16>>8)&0xff;
	buf[1] = (u16   )&0xff;
	
	bt_blocking_send(&buf[0], sizeof(uint16_t));
}
