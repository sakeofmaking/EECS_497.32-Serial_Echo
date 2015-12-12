/******************************************************************************
 * File Name:	Lib.h
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		Header file for lib.c file.
 *
 *  Date	 By:		Changes:
 * --------- ----------	-------------------------------------------------------
 * 21 Jul 02 R Weber	Initial File
 * 10 Feb 04 T Lill		Added 8MHz option for oscillator.  Modified for class project.
 * 03 May 05 T Lill		Removed BYTE and WORD definitions.  Added 8.5MHz clock
 *						option.  Added macros to replace deprecated cbi, sbi 
 *						functions.
 * 15 Dec 06 T Lill		Deleted the multiple clock speed definitions.  Added a 
 *						warning about SET_BIT's behavior under optimization.
 * 31 Oct 07 T Lill		Deleted OSC_FREQ symbol, replace with F_CPU in makefile.
 ******************************************************************************/
#if !defined(LIB_H)		/* Prevents including this file multiple times */
#define LIB_H

#include <avr/io.h>

#define STRLEN 20			/* Maximum string length for conversions */

/* Define memory mapping for commonly used registers */

// Define Port B uses
#define HEARTBEAT_LED_BIT				0x00
#define ERROR_LED_BIT					0x01

/* Memory types */
typedef unsigned char *MEMPTR;

typedef enum {
    FALSE = 0,
    TRUE = 1
} eBooleanType;

/* Macros replacing deprecated functions. */
#define CLEAR_BIT(reg, bit) reg = (reg & (~(1 << bit)))
/*
 * WARNING:  SET_BIT may not work properly if the register
 *		is an I/O PINx register.  It works in this case if
 *		optimization is set to -Os, but it fails if the
 *		optimization is set to -O0.  It has not been tested
 *		for other optimization levels.
 */
#define SET_BIT(reg, bit) reg = (reg | (1 << bit))

#endif /* LIB_H */
