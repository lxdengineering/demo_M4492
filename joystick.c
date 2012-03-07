/****************************************************************************
 *
 * Project: Generic test routines.
 *
 * Copyright by Olimex Ltd. All rights reserved.
 *
 * File: joystick.c
 * Description: Joystick HAL for PIC32-MX460
 * Developer: Dimitar Dimitrov ( dinuxbg,gmail.com )
 *
 * Last change: $Date$
 * Revision: $Revision: 29 $
 * Id: $Id$
 *
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <plib.h>

#include "joystick.h"

#if defined ST7565_M4492_PROTOTYPE_OLIMEX_UEXTPORT
void joys_init(void)
{
	PORTSetPinsDigitalIn(IOPORT_D, BIT_6);	//up
	PORTSetPinsDigitalIn(IOPORT_D, BIT_4);	//left
	PORTSetPinsDigitalIn(IOPORT_D, BIT_13);	//down
	PORTSetPinsDigitalIn(IOPORT_A, BIT_7);	//right
	//PORTSetPinsDigitalIn(IOPORT_D, BIT_5);	//right

	PORTSetPinsDigitalIn(IOPORT_D, BIT_7);	//center
}


enum joys_direction joys_get_direction(void)
{
	if(~mPORTDRead() & (1<<7)) {
		return JOYS_DIR_CENTER;
	} else if(~mPORTDRead() & (1<<6)) {
		if(~mPORTDRead() & (1<<4)) {
			return JOYS_DIR_NW;
		} else if(~mPORTARead() & (1<<7)) {
			return JOYS_DIR_NE;
		} else {
			return JOYS_DIR_N;
		}
	} else if(~mPORTDRead() & (1<<13)) {
		if(~mPORTDRead() & (1<<4)) {
			return JOYS_DIR_SW;
		} else if(~mPORTARead() & (1<<7)) {
			return JOYS_DIR_SE;
		} else {
			return JOYS_DIR_S;
		}
	} else if(~mPORTDRead() & (1<<4)) {
		return JOYS_DIR_W;
	} else if(~mPORTARead() & (1<<7)) {
		return JOYS_DIR_E;
	} else {
		return JOYS_DIR_IDLE;
	}
}
#endif
