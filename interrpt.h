/******************************************************************************
 * File Name:	Interrpt.h
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		Header file for Interrupts.c file.
 *
 *  Date	Changed by:	Changes:
 * -------	-----------	-------------------------------------------------------
 * 12Aug01	R Weber		Initial File
 * 27Aug01	R Weber		Added interrupt for signal generation.
 *  2Sep01	R Weber		Added trap for unused interrupts.
 *  3Mar02  R Weber     Moved definition of vectors to here.
 * 12Feb04	T Lill		Modified for class project.
 ******************************************************************************/
#if !defined(INTERRPT_H)		/* Prevents including this file multiple times */
#define INTERRPT_H
#include <avr/interrupt.h>

/* Interrupt prototypes */
void ISR_InitTimer0(void);

#endif /* INTERRPT_H */
