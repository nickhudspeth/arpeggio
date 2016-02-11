/*************************************************************************
Title:    mcp4725.h - Microchip MCP4725 12-Bit DAC Driver
Author:   Nicholas Morrow <nickhudspeth@gmail.com> http://www.nickhudspeth.com
File:     mcp4725.h
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: STMicroelectronisc STM32F4
License:  GNU General Public License

DESCRIPTION:
    What does this module do? How does it work?

USAGE:
    See Doxygen manual

NOTES:
    None

LICENSE:
    Copyright (C) 2015 Nicholas Morrow

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
#include "stm32f4xx_hal.h"
#include <stdlib.h>

/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
#define VOLTS_PER_OCTAVE 0
#define HERTZ_PER_VOLT 1
#define A0_LOW 0
#define A0_HIGH 1
#define MCP4725_ADDR (0xC0)   // Assumes A2, A1 are 0 and A0 is GND
//#define MCP4725_ADDR (0xC1) // Assumes A2, A1 are 0 and A0 is VCC 
#define MCP4725_PD_NORMAL 0
#define MCP4725_PD_1K 1
#define MCP4725_PD_100K 2
#define MCP4725_PD_500K 3


struct mcp4725 {
    uint16_t current_value; //
    uint8_t transpose;      //
    uint8_t pcs;            // pitch control schema
	  I2C_HandleTypeDef* i2c; // i2c peripheral handle
    uint8_t init;           // dac initialized flag
};


/***********************    FUNCTION PROTOTYPES    ***********************/

/*************************************************************************
 * Function :    mcp4725_init()
 * Purpose  :    initializes and returns a pointer to a mcp4725 structure
 * Input    :    uint8_t pcs - pitch control schema
 * Returns  :    int8_t
 *************************************************************************/
int8_t mcp4725_init(uint8_t pcs, I2C_HandleTypeDef* i2c);


/*************************************************************************
 * Function :    mcp4725_write_fast()
 * Purpose  :    sets the DAC output voltage to match the MIDI note specified
 *               by the argument 'note.'
 * Input    :    uint8_t note - MIDI note for which CV is to be generated
 *               uint8_t pd   - power-down mode selection
 * Returns  :    int8_t      - success
 *************************************************************************/
int8_t mcp4725_write_fast(uint8_t note, uint8_t pd);


/*************************************************************************
 * Function :    mcp4725_write_dac_reg()
 * Purpose  :    sets the DAC output voltage to match the MIDI note specified
 *               by the argument 'note'
 * Input    :    uint8_t note, uint8_t pd
 * Returns  :    int8_t
 *************************************************************************/
int8_t mcp4725_write_dac_reg(uint8_t note, uint8_t pd);


/*************************************************************************
 * Function :    mcp4725_write_dac_reg_and_eeprom()
 * Purpose  :    What does this function do?
 * Input    :    uint8_t note, uint8_t pd
 * Returns  :    int8_t

 *************************************************************************/
int8_t mcp4725_write_dac_reg_and_eeprom(I2C_HandleTypeDef* hi2c, uint8_t note, uint8_t pd);
