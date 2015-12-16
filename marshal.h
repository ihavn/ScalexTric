/*
 * marshal.h
 *
 * Created: 08/12/2015 15:41:18
 *  Author: Peter Skrypalle
 */ 


#ifndef MARSHAL_H_
#define MARSHAL_H_

#include "include/board.h"

void bt_send_u8(uint8_t u8);
void bt_send_u16(uint16_t u16);

#endif /* MARSHAL_H_ */