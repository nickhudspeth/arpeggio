/************************************************************************
Title:
Author:   Nicholas Morrow <nickhudspeth@gmail.com> http://www.nickhudspeth.com
File:     event.c
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware:
License:  GNU General Public License
Usage:    Refer to the header file event.h for a description of the routines.
    See also example test_event.c, if available.
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

#include "event.h"


/*******************    FUNCTION IMPLEMENTATIONS    ********************/
enum event_types event_types_list[] = {
    ALL_EVENTS,
    BUTTON_INPUT,
    ENC_INPUT,
    SLIDER_INPUT,
    MIDI_INPUT
};

struct event *event_create(event_type t)
{
    struct event *ret = (struct event *)calloc(1, sizeof(struct event));
    ret->type    = t;
    ret->used    = 0;
    ret->data_count = 0;
    return ret;
}

void event_destroy(struct event *e)
{
    // int i;
    // for (i = 0; i < (e->data_count); i++)
    // {free(e->data[i]);}  // Deallocate each data container in event
    free(e);
}



//void event_filter(event_type e,
//                  struct event_buffer *buf)
//{
//    int i, j, valid = 0;
//    for (i = 0; i < (buf->len); i++) {
//        for (j = 0; j < NUM_EVENT_TYPES; j++) {
//            /* For each event in the buffer, check its event type against
//               each of the supported event types in the valid_events list. */
//            if (((buf->events[i]->type) & (event_types_list[j]))) {
//                valid = 1; break;
//            }
//        }
//        /* Remove the event if it does not match any of the supported types */
//        if (!valid) { event_remove_from_buffer(buf->events[i], buf); }
//        valid = 0;  // Reset the valid flag and move to the next event
//    }
//    return;
//}


int8_t event_add_data(struct event_data *data, uint8_t index, struct event *e)
{
    if ((index > MAX_EVENT_DATA_SIZE) || (index < 0)) { return -1; }  // Index bounds check
    /* Add data to specified index. Check and return error if data index is full */
    if ((e->data_count) < MAX_EVENT_DATA_SIZE)
		    {
					(e->data)[index] = *data;
					return 0;
				}
    else {
			return -1; /* Data index is full */
		}
    /* Increase the data count if the value is being added to the end of the array*/
    if (index < (e->data_count)) { return 1;}
    else {(e->data_count)++; return 1;}
}


struct event_data *event_wrap_data(event_data_types t, void *data)
{
    struct event_data *wrapper = (struct event_data *)calloc(1, sizeof(struct event_data));
    switch (t) {
        case EVENT_CHAR_T   : {
                wrapper->type = EVENT_CHAR_T;
                wrapper->data.char_v  = (char)data;
                break;
            }
        case EVENT_UINT8_T  : {
                wrapper->type = EVENT_UINT8_T;
                wrapper->data.uint8_v  = (uint8_t)data;
                break;
            }
        case EVENT_UINT16_T : {
                wrapper->type = EVENT_UINT16_T;
                wrapper->data.uint16_v  = (uint16_t)data;
                break;
            }
        case EVENT_UINT32_T : {
                wrapper->type = EVENT_UINT32_T;
                wrapper->data.uint32_v  = (uint32_t)data;
                break;
            }
        case EVENT_INT8_T   : {
                wrapper->type = EVENT_INT8_T;
                wrapper->data.int8_v = (int8_t)data;
                break;
            }
        case EVENT_INT16_T  : {
                wrapper->type = EVENT_INT16_T;
                wrapper->data.int16_v  = (int16_t)data;
                break;
            }
        case EVENT_INT32_T  : {
                wrapper->type = EVENT_INT32_T;
                wrapper->data.int32_v  = (int32_t)data;
                break;
            }
        case EVENT_FLOAT_T  : {
                wrapper->type = EVENT_FLOAT_T;
                wrapper->data.float_v  = *(float*)data;
                break;
            }
        case EVENT_DOUBLE_T : {
                wrapper->type = EVENT_DOUBLE_T;
                wrapper->data.double_v  = *(double*)data;
                break;
            }
    }
    return wrapper;
}

void event_set_type(struct event *e, event_type t)
{
	e->type = t;
}
