/*************************************************************************
Title:    Interface library for Grey code rotary encoders
Author:   Nicholas Morrow <nicholas.morrow@tangibleinstruments.com>
File:     encoders.c
Software: libopencm3
Hardware: AVR ATMega2560, or software compatible
License:  GNU General Public License

DESCRIPTION:
    Provides data structure, with create/destroy/read/update routines for easy
    interface with Alpha 318-ENC130175F-12PS rotary encoders from Sparkfun.

USAGE:
    Refer to the header file encoders.h for a description of the routines.

NOTES:
    Product listing - https://www.sparkfun.com/products/9117
    Datasheet - https://www.sparkfun.com/datasheets/Components/TW-700198.pdf

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
#include <stdlib.h>
#include <stdio.h>
#include "gpio.h"
#include "encoder.h"



/*********************    CONSTANTS AND MACROS    **********************/
#ifndef NULL
#define NULL ((void*)0)
#endif



/***********************    GLOBAL VARIABLES    ************************/

static int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

const unsigned char ttable[7][4] = {
{0x0, 0x2, 0x4, 0x0}, {0x3, 0x0, 0x1, 0x10},
{0x3, 0x2, 0x0, 0x0}, {0x3, 0x2, 0x1, 0x0},
{0x6, 0x0, 0x4, 0x0}, {0x6, 0x5, 0x0, 0x20},
{0x6, 0x5, 0x4, 0x0},
};

/*******************    FUNCTION IMPLEMENTATIONS    ********************/

struct encoder *encoder_init(uint8_t id, GPIO_TypeDef *a, uint16_t pa,
														 GPIO_TypeDef *b, uint16_t pb, 
														 GPIO_TypeDef *btn, uint16_t pbtn)
{
    struct encoder *enc = (struct encoder *)calloc(1, sizeof(struct encoder));
    enc->id     = id;
    enc->old_AB = 0;
    enc->scale  = 1;
    enc->gpio_a   = a;
    enc->gpio_b   = b;
    enc->gpio_btn = btn;
	  enc->pin_a = pa;
	  enc->pin_b = pb;
		enc->pin_btn = pbtn;
    return enc;
}

void encoder_destroy(struct encoder *enc)
{
    free(enc);
}

void encoder_set_scale(struct encoder *enc, uint32_t val)
{
    enc->scale = val;
}


int8_t encoder_read(struct encoder *enc)
{
    uint8_t states = 0;
    uint8_t pinA = HAL_GPIO_ReadPin(enc->gpio_a, enc->pin_a);
    uint8_t pinB = HAL_GPIO_ReadPin(enc->gpio_b, enc->pin_b);
    if(pinA == 1){
      states = (pinB << 1) | pinA;
    }
    else if(pinB == 1){
      states = (pinA << 1) | pinB;
    }
    enc->old_AB <<= 2;                   //remember previous state
    enc->old_AB |= ( states & 0x03 );  //add current state
    return (enc->scale) * ( enc_states[(enc->old_AB & 0x0f )]); //scale value and return
}

