/*************************************************************************
Title:    encoder.h -
Author:   Nicholas Morrow <nicholas.morrow@tangibleinstruments.com>
File:     encoder.h
Software: libopencm3
Hardware:
License:  GNU General Public License

DESCRIPTION:
    What does this module do? How does it work?

USAGE:
    See Doxygen manual

NOTES:
    None

LICENSE:
    Copyright (C) 2015 Tangible Instruments, LLC

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*************************************************************************/

/**********************    INCLUDE DIRECTIVES    ***********************/
#define ENCODER_BOUNCE_CHECK_INTERVAL 25
#define ENCODER_BOUNCE_WAIT_INTERVAL 25

#include "stm32f4xx_hal.h"

/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/

 struct encoder{
    uint8_t old_AB;
    uint8_t scale;
	  uint16_t pin_a;
	  uint16_t pin_b;
	  uint16_t pin_btn;
    uint32_t id;
    GPIO_TypeDef * gpio_a;
    GPIO_TypeDef * gpio_b;
    GPIO_TypeDef * gpio_btn; 
};


/***********************    FUNCTION PROTOTYPES    ***********************/

/*************************************************************************
Function: encoder_init()
Purpose:  Creates, initializes, and returns a pointer to an encoder object.
Input:    uint8_t id  - Numerical identifier for encoder.
          uint32_t a   - Pointer to GPIO object representing encoder's pin A
          uint32_t b   - Pointer to GPIO object representing encoder's pin B
          uint32_t btn - Pointer to GPIO object representing encoder's button
Returns:  struct encoder*
**************************************************************************/
struct encoder *encoder_init(uint8_t id, GPIO_TypeDef *a, uint16_t pa,
														 GPIO_TypeDef *b, uint16_t pb, 
														 GPIO_TypeDef *btn, uint16_t pbtn);

/*************************************************************************
Function: encoder_destroy()
Purpose:  Destroys an instance of an encoder object
Input:    struct encoder* enc - Pointer to encoder object
Returns:  void
**************************************************************************/
void encoder_destroy(struct encoder *enc);

/*************************************************************************
Function: encoder_set_scale()
Purpose:  Sets the encoder output scaling factor.
          EX: If the encoder's scale is set to 5, and the encoder moves by
          one position forward, then encoder_read() will show an advance of
          5 positions since the last measurement.
Input:    struct encoder *enc - Pointer to encoder object
          uint32_t val   - New value for encoder's scale attribute
Returns:  void
**************************************************************************/
void encoder_set_scale(struct encoder *enc, uint32_t val);


/*************************************************************************
Function: encoder_read()
Purpose:  returns the difference in encoder position values between current
          and previous measurements.
Input:    struct encoder* enc
Returns:  int8_t - SIGNED integer representing the difference in position
          since the last measurement.
**************************************************************************/
int8_t encoder_read(struct encoder *enc);
