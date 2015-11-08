/*! @file board.h
@brief Board drivers for the ScalexTric Main Board.

@author iha

@defgroup  board_drivers Drivers for the ScalexTric Main Board.
@{
@brief A driver selection for the MCU board.

@note The functions are NOT protected against interrupts!

@defgroup board_init Board driver initialization.
@brief How to initialize the Board drivers.

@defgroup board_public_function Public Board Functions
@brief Public Board functions.

Here you you will find the functions you will need.

@defgroup board_public Public Declarations, variables etc.
@brief Declarations, variables etc. that are private for the Board Driver implementation.

@defgroup board_return Board Driver Return codes
@brief Codes returned from Board Driver functions.
@}
*/

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

/**----------------------------------------------
@ingroup board_init
@brief Board Driver initialization.

Initializes the Board Drivers.
------------------------------------------------*/
void init_main_board();

/**----------------------------------------------
@ingroup board_public_function
@brief Manipulate with the Car Horn.

@param[in] state 0: turn horn off, 1: turn horn on.
------------------------------------------------*/
void set_horn(uint8_t state);

/**----------------------------------------------
@ingroup board_public_function
@brief Manipulate with the Car Head Light.

@param[in] state 0: turn head light off, 1: turn head light on.
------------------------------------------------*/
void set_head_light(uint8_t state);

/**----------------------------------------------
@ingroup board_public_function
@brief Manipulate with the Car Brake Light.

@param[in] state 0: turn brake light off, 1: turn brake light on.
------------------------------------------------*/
void set_brake_light(uint8_t state);

/**----------------------------------------------
@ingroup board_public_function
@brief Set speed of the Car Motor.

@param[in] speed_percent -10 to 100 Negative values means brake/reverse.
------------------------------------------------*/
void set_motor_speed(int8_t speed_percent);

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest X acceleration.

@return X-Acceleration [g].
------------------------------------------------*/
float get_x_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Y acceleration.

@return Y-Acceleration [g].
------------------------------------------------*/
float get_y_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Z acceleration.

@return Z-Acceleration [g].
------------------------------------------------*/
float get_z_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw X acceleration.

@return Raw X-Acceleration in binary value.
------------------------------------------------*/
int16_t get_raw_x_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw Y acceleration.

@return Raw Y-Acceleration in binary value.
------------------------------------------------*/
int16_t get_raw_y_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw Z acceleration.

@return Raw Z-Acceleration in binary value.
------------------------------------------------*/
int16_t get_raw_z_accel();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest X rotation.

@return X-rotation [degrees/s].
------------------------------------------------*/
float get_x_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Y rotation.

@return Y-rotation [degrees/s].
------------------------------------------------*/
float get_y_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Z rotation.

@return Z-rotation [degrees/s].
------------------------------------------------*/
float get_z_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw X rotation.

@return Raw X-rotation in binary value.
------------------------------------------------*/
int16_t get_raw_x_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw Y rotation.

@return Raw Y-rotation in binary value.
------------------------------------------------*/
int16_t get_raw_y_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief Get newest Raw Z rotation.

@return Raw Z-rotation in binary value.
------------------------------------------------*/
int16_t get_raw_z_rotation();

/**----------------------------------------------
@ingroup board_public_function
@brief	Get tacho counts measured
		since last time the function is called.
		
@note	If the counter counts more than 65535 pulses
		between calls of this function, the result will be wrong.	

@return Tacho counts since last call [0-65535].
------------------------------------------------*/
uint16_t get_tacho_count();

/**----------------------------------------------
@ingroup board_public_function
@brief Manipulate with the RESET signal on the Blue tooth module.

After reset/power-on the RESET line is held low/active. 
@note Before the Blue tooth module can be used the reset must be set inactive.

@param[in] state 0: Inactivate RESET, 1: Activate RESET.
------------------------------------------------*/
void set_bt_reset(uint8_t state);
#endif /* BOARD_H_ */