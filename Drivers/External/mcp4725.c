/************************************************************************
Title:    Microchip MCP4725 12-Bit DAC Driver
Author:   Nicholas Morrow <nickhudspeth@gmail.com> http://www.nickhudspeth.com
File:     mcp4725.c
Software: ARM-GCC 4.1, libopencm3
Hardware: STMicroelectronics STM32F4
License:  GNU General Public License
Usage:    Refer to the header file mcp4725.h for a description of the routines.
    See also example test_mcp4725.c, if available.
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

************************************************************************/

/**********************    INCLUDE DIRECTIVES    ***********************/

#include "mcp4725.h"


/*********************    CONSTANTS AND MACROS    **********************/


/***********************    GLOBAL VARIABLES    ************************/
uint16_t pcs_tables[2][48] = {0};
//extern struct mcp4725 *dac;
struct mcp4725 *dac;
/*******************    FUNCTION IMPLEMENTATIONS    ********************/

/*************************************************************************
 * Function :    mcp4725_init()
 * Purpose  :    initializes and returns a pointer to a mcp4725 structure
 * Input    :    uint8_t pcs - pitch control schema
 * Returns  :    int8_t
 *************************************************************************/
int8_t mcp4725_init(uint8_t pcs, I2C_HandleTypeDef* i2c)
{
    /* Initialize dac structure */
    dac = (struct mcp4725 *)calloc(1, sizeof(struct mcp4725));
    if (dac == NULL) {
        return -1;
    }
    if (i2c == 0) {
        return -1;
    }
    dac->pcs = pcs;
    dac->i2c = i2c;
    /* Initialize pitch control schema tables */
    for (uint8_t i = 0; i < 48; i++) {
        pcs_tables[0][i] = (uint16_t)(0.5f + (float)i * 4096.0f / 48.0f);
    }
    dac->init = 1;
    return 0;
}

/*************************************************************************
 * Function :    mcp4725_write_fast()
 * Purpose  :    sets the DAC output voltage to match the MIDI note specified
 *               by the argument 'note.'
 * Input    :    uint8_t note - MIDI note for which CV is to be generated
 *               uint8_t pd   - power-down mode selection
 * Returns  :    uint8_t      - success
 *************************************************************************/
int8_t mcp4725_write_fast(uint8_t note, uint8_t pd)
{
	  uint8_t data[2] = {0};
    int8_t idx = 0;
    if (dac->init == 0) { return -1; } /* return error if DAC uninitialized */
    if ((note < 33) || (note > 128)) { return -1; }
    idx = note - 33 + dac->transpose;
    if (idx < 0 ) { idx = 0;}
    if (idx > 47) {idx = 47;}
    dac->current_value = pcs_tables[dac->pcs][idx];
    switch (pd) {
        case MCP4725_PD_NORMAL:
            data[0] = (dac->current_value >> 8);
            break;
        case MCP4725_PD_1K:
            data[0] = (dac->current_value >> 8) | ( 1 << 4);
            break;
        case MCP4725_PD_100K:
            data[0] = (dac->current_value >> 8) | ( 1 << 5);
            break;
        case MCP4725_PD_500K:
            data[0] = (dac->current_value >> 8) | ( 1 << 4 ) | (1 << 5);
            break;
        default:
            data[0] = dac->current_value >> 8;
            break;
    }
		data[1] = (uint8_t)dac->current_value;
		HAL_I2C_Master_Transmit(dac->i2c, MCP4725_ADDR<<1 , data, 2, 10);
    return 0;
}

/*************************************************************************
 * Function :    mcp4725_write_dac_reg()
 * Purpose  :    sets the DAC output voltage to match the MIDI note specified
 *               by the argument 'note'
 * Input    :    uint8_t note, uint8_t pd
 * Returns  :    uint8_t
 *************************************************************************/
int8_t mcp4725_write_dac_reg(uint8_t note, uint8_t pd)
{
	  uint8_t data[3] = {0};
    int8_t idx = 0;
    if (dac->init == 0) { return -1; } /* return error if DAC uninitialized */
    if ((note < 33) || (note > 128)) { return -1; }
    idx = note - 33 + dac->transpose;
    if (idx < 0 ) { idx = 0;}
    if (idx > 47) {idx = 47;}
    dac->current_value = pcs_tables[dac->pcs][idx];
    switch (pd) {
        case MCP4725_PD_NORMAL:
            data[0] = (1 << 6);
            break;
        case MCP4725_PD_1K:
            data[0] = (1 << 6) | ( 1 << 1);
            break;
        case MCP4725_PD_100K:
            data[0] = (1 << 6) | ( 1 << 2);
            break;
        case MCP4725_PD_500K:
            data[0] = (1 << 6) | ( 1 << 1 ) | (1 << 2);
            break;
        default:
            data[0] = (1 << 6);
            break;
    }
		data[1] = dac->current_value >> 4;
		data[2] = dac->current_value << 4;
		HAL_I2C_Master_Transmit(dac->i2c, (MCP4725_ADDR << 1), data, 3, 10);
    return 0;
}

/*************************************************************************
 * Function :    mcp4725_write_dac_reg_and_eeprom()
 * Purpose  :    What does this function do?
 * Input    :    uint8_t note, uint8_t pd
 * Returns  :    int8_t

 *************************************************************************/
int8_t mcp4725_write_dac_reg_and_eeprom(I2C_HandleTypeDef* hi2c, uint8_t note, uint8_t pd)
{
	  uint8_t data[3];
    int8_t idx = 0;
    if (dac->init == 0)
		{ 
			return -1; /* return error if DAC uninitialized */
		} 
    if ((note < 33) || (note > 128))
		{
			return -1; 
		}
    idx = note - 33 + dac->transpose;
    if (idx < 0 )
		{ 
			idx = 0;
		}
    if (idx > 47) 
		{
			idx = 47;
		}
    dac->current_value = pcs_tables[dac->pcs][idx];
    switch (pd)
		{
        case MCP4725_PD_NORMAL:
            data[0] = (1 << 6) | (1 << 5);
            break;
        case MCP4725_PD_1K:
            data[0] = (1 << 6) | (1 << 5) | ( 1 << 1);
            break;
        case MCP4725_PD_100K:
            data[0] = (1 << 6) | (1 << 5) | ( 1 << 2);
            break;
        case MCP4725_PD_500K:
            data[0] = (1 << 6) | (1 << 5) | ( 1 << 1 ) | (1 << 2);
            break;
        default:
            data[0] = (1 << 6) | (1 << 5);
            break;
    }
		data[1] = dac->current_value >> 4; // upper 8 bits of output value
		data[2] = dac->current_value << 4; // lower 8 bits of output value
		HAL_I2C_Master_Transmit(dac->i2c,(MCP4725_ADDR << 1), data, 3, 10);
    return 0;
}
