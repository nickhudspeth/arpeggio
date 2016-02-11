/*************************************************************************
Title:    event.h - Generic Event Handling Routines for Arpeggio
Author:   Nicholas Morrow <nickhudspeth@gmail.com> http://www.nickhudspeth.com
File:     event.h
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: Independent
License:  GNU General Public License

DESCRIPTION:
    This module provides event handling routines for implementation in a
    finite state machine (FSM) with event-driven transitions or similar model,
    along with generic data structures for events, creators and destructors for
    those data structures, and a memory-optimized (variable-length) FIFO buffer.

    Provisions are also given for filtering events based on specified type-
    selection criteria. This is useful in a FSM implementation where only a
    subset of the set of event types are relevant to a particular state.

    Reading an event removes it from the buffer and deallocates its memory
    automatically.

USAGE:


    ----GLOBAL----

    Redefine enum event_types_e from event.h to rename the event types as necessary.
    Up to 16 separate event types are supported.

            enum event_types {
                ALL_EVENTS   = (1 << 0),
                BUTTON_INPUT = (1 << 1),
                ENC_INPUT    = (1 << 2),
                SLIDER_INPUT = (1 << 3),
                MIDI_INPUT   = (1 << 4),
                MY_EVENT_1   = (1 << 5),
                MY_EVENT_2   = (1 << 6)
            };
};

    Create one or more event buffers

            struct event_buffer *my_buffer = event_create_buffer();


    ----INTERRUPT ROUTINE----

    To pass some information about the event to the states, create some events, and add data to the events.
    (my_char, my_float, my_uint8, my_uint32 are variables that are being used as event data)

            struct event my_event_1 = event_create(MY_EVENT_1);
            struct event my_event_2 = event_create(MY_EVENT_2);
            event_add_data(event_wrap_data(EVENT_CHAR_T,my_char), 0, my_event_1);  // Store my_char as the first element in data array of my_event_1
            event_add_data(event_wrap_data(EVENT_FLOAT_T,my_float), 1, my_event_1);
            event_add_data(event_wrap_data(EVENT_UINT8_T,my_uint8), 0, my_event_1);
            event_add_data(event_wrap_data(EVENT_INT32_T,my_uint32), 1, my_event_1);

    Append the events to the event buffer
            event_add_to_buffer(my_event_1, my_buffer);
            event_add_to_buffer(my_event_2, my_buffer);


    ----STATE CALLBACK FUNCTION----

        void someState(void)
        {
            enum event_types valid_events |= (BUTTON_INPUT | ENC_INPUT | MIDI_INPUT); // Restrict the set of relevant events for the mode
                                                                                // If all event types are valid, just use " valid_events |= ALL_EVENTS; "

            // Just check for new events in the processing function, memory management is handled automatically.
            // Events are removed from the buffer after being read; no events will be read twice.

            if((struct event *e = event_get_next(my_buffer, valid_events)))
            {
                switch(e->type)
                {
                    case MY_EVENT_TYPE_1:
                        // Do something
                        break;
                    case MY_EVENT_TYPE_2:
                        // Do something else
                        break;
                    case MY_EVENT_TYPE_3:
                    case MY_EVENT_TYPE_4:
                    case MY_EVENT_TYPE_5:
                }
            }
        }


NOTES:
    Function event_add_data() does not currently support adding arrays as event data.

TO-DO:
    Write creators & destructors for event type lists.
    Write "add type to list" function for event type lists.

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
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"

/**************    CONSTANTS, MACROS, & DATA STRUCTURES    ***************/
#define MAX_EVENT_BUFFER_SIZE 10   // Number of elements in FIFO
#define MAX_EVENT_DATA_SIZE 4  // Number of elements in event data array


typedef uint16_t event_type;
//typedef int16_t event_data_type;

/* NOTE: Default enum size on AVR is 16-bits. */
enum event_types {
    ALL_EVENTS   = (1 << 0),
    BUTTON_INPUT = (1 << 1),
    ENC_INPUT    = (1 << 2),
    SLIDER_INPUT = (1 << 3),
    MIDI_INPUT   = (1 << 4),
};
#define NUM_EVENT_TYPES 5   /* Update this number to match the number of
                               event types defined above */


/* Classifier for all possible event data formats. See struct event_data */
typedef enum {
    EVENT_CHAR_T = 0,
    EVENT_UINT8_T,
    EVENT_UINT16_T,
    EVENT_UINT32_T,
    EVENT_INT8_T,
    EVENT_INT16_T,
    EVENT_INT32_T,
    EVENT_FLOAT_T,
    EVENT_DOUBLE_T
} event_data_types;

/* Data container for event data, use of a "tagged union" allows
   the struct event.data[] array to hold mixed-type values*/
struct event_data {
    event_data_types type;
    union {
        char char_v;
        uint8_t uint8_v;
        uint16_t uint16_v;
        uint32_t uint32_v;
        int8_t int8_v;
        int16_t int16_v;
        int32_t int32_v;
        float float_v;
        double double_v;
    } data;
};

/* Container for generic input events */
struct event {
    event_type type;  // Event type classification
    int8_t volatile used;  // Indicates whether event has been read or not
    uint8_t volatile data_count;  // Number of filled elements in data array
    struct event_data data[MAX_EVENT_DATA_SIZE];  // Array to hold event-specific data
};


/***********************    FUNCTION PROTOTYPES    ***********************/

/*************************************************************************
Function : event_create()
Purpose  : Creates a new event
Input    : event_type t : Type of event to create
Returns  : struct event* : Pointer to newly created event
Note     : Could cause memory leak if events are created and not added to
           the event buffer, because events are only destroyed in garbage
           collection performed by event_get_next().
           Consider passing data arguments to create event, and automatically
           adding events to buffer in this function.
**************************************************************************/
struct event *event_create(event_type t);


/*************************************************************************
Function : event_destroy()
Purpose  : Destroys an event to return memory to system.
Input    : struct event* event : Pointer to event to be destroyed
Returns  : void
Note     : NOT to be called explicitly. Used in garbage collection by
           event_get_next().
**************************************************************************/
void event_destroy(struct event *e);


/*************************************************************************
Function : event_create_buffer()
Purpose  : Creates a new event buffer
Input    : void
Returns  : struct event_buffer*: Pointer to new event buffer
**************************************************************************/
struct event_buffer *event_create_buffer(void);


/*************************************************************************
Function : event_destroy_buffer()
Purpose  : Destroys an event buffer to return memory to system
Input    : struct event_buffer* buf
Returns  : void
**************************************************************************/
void event_destroy_buffer(struct event_buffer *buf);


/*************************************************************************
Function : event_add_to_buffer()
Purpose  : Add a new event to the circular event buffer
Input    : struct event *e              : Pointer to event to be added to
                                          event buffer buf_old
         : struct event_buffer *buf_old : Pointer to event buffer
Returns  : void
**************************************************************************/
void event_add_to_buffer(struct event *e, struct event_buffer *buf_old);


/*************************************************************************
Function : event_remove_from_buffer()
Purpose  : Removes a specified event from a given event buffer
Input    : struct event* event, struct event_buffer *obuf
Returns  : void
**************************************************************************/
void event_remove_from_buffer(struct event *e, struct event_buffer *buf);


/*************************************************************************
Function : event_filter()
Purpose  : Removed unsupported event types from the event buffer
Input    : struct event_type_list* valid_events : List of supported event types
           struct event_buffer *buf                : Pointer to buffer to filter
Returns  : void
**************************************************************************/
void event_filter(event_type e, struct event_buffer *buf);


/*************************************************************************
Function : event_get_next()
Purpose  : What does this function do?
Input    : struct event_buffer *buf                 : Pointer to the event buffer
           struct event_type_list* valid_events : List of supported event types
Returns  : struct event* : Pointer to the next valid event in the buffer
**************************************************************************/
struct event *event_get_next(struct event_buffer *buf, event_type e);


/*************************************************************************
Function :  event_add_data()
Purpose  :  Populates an event's data array with given data
Input    :  event_data_type data    : data to be added to event data array
            uint8_t index           : index in event's data array where data should be stored
            event_type *e           : pointer to event to be modified
Returns  :  bool : zero on success, +1 if data array is full
**************************************************************************/
int8_t event_add_data(struct event_data *data, uint8_t index, struct event *e);


/*************************************************************************
Function :  event_wrap_data()
Purpose  :  Packages data for storage in struct event structure
Input    :  event_type t : data type being wrapped
            void *data         : pointer to the data to be wrapped
Returns  :  struct event_data* : pointer to data container for use in struct event
**************************************************************************/
struct event_data *event_wrap_data(event_data_types t, void *data);


/*************************************************************************
Function :  event_set_type()
Purpose  :  Allows configuration of the type variable for an event
Input    :  struct event *e    : pointer to the event to be modified
            event_type t       : type to assign to event e
Returns  :  struct event_data* : pointer to data container for use in struct event
**************************************************************************/
void event_set_type(struct event *e, event_type t);
