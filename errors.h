/******************************************************************************
 * File Name:	errors.h
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		Contains enumeration for error values. 
 *
 *  Date		Changed by:	Changes:
 * -------		-----------	-------------------------------------------------------
 * 16 Mar 02	R Weber		Original file.
 * 12 Feb 04	T Lill		Modified for class project.
 * 31 Oct 07 	T Lill		Deleted OSC_FREQ symbol, replace with F_CPU in makefile.
 ******************************************************************************/
#if !defined(ERRORS_H)		/* Prevents including this file multiple times */
#define ERRORS_H

typedef enum
{
    NO_ERROR = 0,

    // Overrun faults
    SLOW_TASK_OVERRUN,
    MEDIUM_TASK_OVERRUN,
    FAST_TASK_OVERRUN,

    // SCI faults
    SCI_TX_BUFFER_OVERFLOW,
	SCI_RX_BUFFER_OVERFLOW,
	SCI_RX_FRAME,
	SCI_RX_DATA_OVERRUN,
	SCI_RX_PARITY,

    /* SPI Faults */
    SPI_WRITE_COLLISION,
    SPI_MODE_FAULT,
    SPI_PREV_TX_INCOMPLETE,

    /* Unused Interrupts */
    UNUSED_INTERRUPT,

	// Parameter errors
    INVALID_PARAMETER,
    PARAMETER_OUT_OF_RANGE
} eErrorType;

/* Function Prototypes */
void ReportError(eErrorType);

#endif /* ERRORS_H */
