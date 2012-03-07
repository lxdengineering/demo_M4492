/****************************************************************************
 *
 * Project: Generic test routines.
 *
 * Copyright by Olimex Ltd. All rights reserved.
 *
 * File: joystick.h
 * Description: Joystick HAL for PIC32-MX460
 * Developer: Dimitar Dimitrov ( dinuxbg,gmail.com )
 *
 * Last change: $Date$
 * Revision: $Revision: 29 $
 * Id: $Id$
 *
 ****************************************************************************/

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef EIO
  #define EIO	5
#endif

enum joys_direction {
	JOYS_DIR_IDLE = 0,
	JOYS_DIR_E,
	JOYS_DIR_NE,
	JOYS_DIR_N,
	JOYS_DIR_NW,
	JOYS_DIR_W,
	JOYS_DIR_SW,
	JOYS_DIR_S,
	JOYS_DIR_SE,
	JOYS_DIR_CENTER
};

extern void joys_init(void);

extern enum joys_direction joys_get_direction(void);

#ifdef __cplusplus
}
#endif

#endif	/* JOYSTICK_H */

