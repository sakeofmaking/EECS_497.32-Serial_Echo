/******************************************************************************
 * File Name:	errors.c
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		Contains library-like functions.
 *
 *  Date	 By:		Changes:
 * --------- ----------	-------------------------------------------------------
 * 21 Jul 02 R Weber	Initial file
 * 12 Feb 04 T Lill	Modified for class project.
 * 09 May 05 T Lill	Removed deprecated functions.
 ******************************************************************************/
#include "lib.h"
#include "errors.h"

eErrorType SystemError = NO_ERROR;

/*****************************************************************************
 * Error handler
 *****************************************************************************/
void ReportError(eErrorType iError)
{
	/* Record error and set error LED */
	SystemError = iError;
	CLEAR_BIT(PORTB, ERROR_LED_BIT);
}
