/******************************************************************************
 * File Name:	serial.c
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems
 * Author:		Robert Weber
 * Purpose:		Contains Serial Communications Interface (SCI) functions.
 *
 *  Date		Changed by	Changes
 * ---------	----------	-------------------------------------------------------
 * 18F eb 02	R Weber		Initial file
 * 02 Mar 02	R Weber     Added receive interrupt capability.
 *							Also changed TX method to use a circular buffer.
 * 10 Feb 04	T Lill		Modified welcome message.  Modified for class project.
 * 17 Nov 04	T Lill		Editorial changes only.
 * 03 May 05	T Lill		Removed deprecated functions.  Updated symbols for
 *							latest compiler release.
 * 05 Apr 06	T Lill		Removed obsolete signal.h header.
 * 25 May 06	T Lill		Replaced SIGNAL with ISR.
 * 31 Oct 07 	T Lill		Deleted OSC_FREQ symbol, replace with F_CPU in makefile.
 * 17 Oct 10	T Lill		Modified program for ATmega2560 processor.
 ******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib.h"
#include "serial.h"
#include "errors.h"

// Calculate Baud Rate values. Note that UBRR_VALUE should be < 4095. 
// No check is made to verify this.
// For F_CPU = 8 MHz, UBBR = 207, which is OK.
#define BAUD_RATE			2400
#define UART_CLOCK_DIVIDER	16
#define UBRR_VALUE 		(((F_CPU/UART_CLOCK_DIVIDER)/BAUD_RATE) - 1)

/* Macros for moving character buffer pointers */
#define INC_CIRC_BUFFER_PTR(ptr, Addr, Length)	            \
    ((ptr >= Addr + Length - 1) ? Addr : ptr+1)

/*
 * Define output string variables.  The Head and Tail pointers - but not the
 * data being pointed to - need to be volatile.
 */
static char zOutputChars[MAX_OUT_STR_SIZE];
static char * volatile ptrOutputCharHead;		
static char * volatile ptrOutputCharTail;

/* Define input string variables */
static char zInputChars[MAX_IN_STR_SIZE];
static char * volatile ptrInputCharHead;
static char * volatile ptrInputCharTail;


/******************************************************************************
 * Initialize the SCI interface.  Use channel 0.
 ******************************************************************************/
void SCIInitialize(void)
{
	// Set the baud rate 
	UBRR0 = UBRR_VALUE;
	
	/* Setup the frame format
	 * UCSRC: 
	 *   Bit 7: UMSEL01 = 0, Aynchronous operation
	 *       6: UMSEL00 = 0
	 *       5: UPM01   = 1, Parity enabled
	 *       4: UPM00   = 1, Odd Parity
	 *       3: USBS0   = 0  1 stop bit
	 *       2: UCSZ01  = 1, 8 data bits 
	 *       1: UCSZ00  = 1
	 *       0: UCPOL   = 0, Not used for asynchronous mode
	 */
	UCSR0C  = _BV(UCSZ01) | _BV(UCSZ00) | _BV(UPM01) | _BV(UPM00);
 
	/* UCSR0B: 
	 *   Bit 7: RXCIE0 = 1, RX interrupt enabled
	 *       6: TXCIE0 = 0, TX Complete interrupt disabled 
	 *       5: UDRIE0 = 0, Data Reg Empty interrupt disabled. This will be
	 * 						enabled when we're ready to transmit.
	 *       4: RXEN0  = 1, Receiver enabled
	 *       3: TXEN0  = 1, Transmitter enabled
	 *       2: UCSZ02 = 0, 8 data bits (see UCSR0C)
	 *       1: RXB80  = 0, Read-only bit
	 *       0: TXB80  = 0, No 9th data bit
	 */
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);

	/* UCSR0A: Error Flags
	 *   Bit 7: RXC0  = 0, Receive complete
	 *       6: TXC0  = 0, Transmit complete
	 *       5: UDRE0 = 0, Read-only bit. Data register empty flag
	 *       4: FE0   = 0, Read-only bit. Frame error flag
	 *       3: DOR0  = 0, Read-only bit. Data overrun flag
	 *       2: UPE0  = 0, Read-only bit. Parity error flag
	 *       1: U2X0  = 0, Do not want to double TX rate
	 *       0: MPCM0 = 0, No multi-processor mode
	 */
	UCSR0A = 0;

   /* Initialize the pointers to the character buffers. New characters to be
     * received or transmitted are  added using the Head pointer. When characters
     * retrieved by an outside program or transmitted by the transmitter, they're
     * removed from the buffers using the Tail pointer. */
    ptrInputCharHead    = zInputChars;
    ptrInputCharTail    = zInputChars;
    ptrOutputCharHead   = zOutputChars;
    ptrOutputCharTail   = zOutputChars;

    // Display start-up greeting
    SCIWriteString_P(PSTR("Welcome to the world of serial communications!\n\r"));
}
 

/*****************************************************************************
 * Interrupt Handler for Transmitting data
 * Leave interrupts disabled during this ISR so we don't interrupt ourselves,
 * bypassing our checks.
 *****************************************************************************/
ISR(USART0_UDRE_vect)
{
	char TxData;
	    
	// Disable UDR interrupt, and enable global interrupts
	CLEAR_BIT(UCSR0B, UDRIE0);
	sei();

	// Save value to transmit, so we can send it later in the ISR.
	TxData = *ptrOutputCharTail;

	/* Move pointer to next character */            
	ptrOutputCharTail = INC_CIRC_BUFFER_PTR(ptrOutputCharTail, 
						zOutputChars, MAX_OUT_STR_SIZE);

	// Send next character. We do this as close to the end of the ISR as
	// possible.
    UDR0 = TxData;

	/* Check to see if we've just transmitted the last character.
	 * If not, enable the interrupt. */
	if (ptrOutputCharTail != ptrOutputCharHead)        
	{
		SET_BIT(UCSR0B, UDRIE0);
	}
}

/*****************************************************************************
 * Interrupt Handler for Receiving data
 *
 * ASSIGNMENT:  Implement this receive ISR
 *
 *****************************************************************************/
ISR(USART0_RX_vect)
{
	unsigned char status;
	char *ptrPrev;

	/* First, since UDR0 resets the error flags */
	status = UCSR0A;

	/* Append character to input string */
	*ptrInputCharHead = UDR0;

	/* Check for receive buffer overflow */
	if(INC_CIRC_BUFFER_PTR(ptrInputCharHead, zInputChars, MAX_IN_STR_SIZE) != ptrInputCharTail){

		/* Move pointer to next character */
		ptrPrev = ptrInputCharHead;
		ptrInputCharHead = INC_CIRC_BUFFER_PTR(ptrInputCharHead, zInputChars, MAX_IN_STR_SIZE);

	} else{
		ReportError(SCI_RX_BUFFER_OVERFLOW);
	}

	/* Check for input character errors */
	if(status & (1<<FE0)){
		ReportError(SCI_RX_FRAME);
	}
	if(status & (1<<DOR0)){
		ReportError(SCI_RX_DATA_OVERRUN);
	}
	if(status & (1<<UPE0)){
		ReportError(SCI_RX_PARITY);
	}


}

/******************************************************************************
 * Outputs the specified string to the RS-232 port.
 ******************************************************************************/
int SCIWriteString(char *Str)
{
	int iReturnCode = 0;    /* Assume success */
	eBooleanType bCopyComplete = FALSE;

	/* Check to see if transmit buffer is full */
	while ((INC_CIRC_BUFFER_PTR(ptrOutputCharHead, zOutputChars,
								 MAX_OUT_STR_SIZE) != ptrOutputCharTail) &&
			(bCopyComplete == FALSE))

	{   /* Copy string to transmit to the output buffer */
		if (*Str != '\0')
		{
			*ptrOutputCharHead = *Str;
 
			/* Move pointer to next character */
			ptrOutputCharHead = INC_CIRC_BUFFER_PTR(ptrOutputCharHead, 
													zOutputChars,
													MAX_OUT_STR_SIZE);
			++Str;
		}
		else
		{   /* Done copying characters */
			bCopyComplete = TRUE;
		}
	}   /* end of copying string */

	/* Enable transmitter interrupt */
	SET_BIT(UCSR0B, UDRIE0);

	/* See if the reason we stopped copying the string was because the 
	 * buffer was full. Note that the buffer being full may no longer be the
	 * case, even if it was true when copying, so we have to use our complete
	 * flag. */
	if (bCopyComplete == FALSE)
	{   /* Prematurely terminated copying */
		ReportError(SCI_TX_BUFFER_OVERFLOW);
		iReturnCode = -1;
	}

	return iReturnCode;
}

/******************************************************************************
 * Outputs the specified string, stored in program space, to the RS-232 port.
 ******************************************************************************/
int SCIWriteString_P(PGM_P Str_P)
{
	int iReturnCode = 0;    /* Assume success */
	eBooleanType bCopyComplete = FALSE;
	char Char_P;
	
	/* Check to see if transmit buffer is full */
	while ((INC_CIRC_BUFFER_PTR(ptrOutputCharHead, zOutputChars,
								 MAX_OUT_STR_SIZE) != ptrOutputCharTail) &&
			(bCopyComplete == FALSE))

	{   /* Copy string to transmit to the output buffer */
		Char_P = pgm_read_byte(Str_P);
		if (Char_P != '\0')
		{
			*ptrOutputCharHead = Char_P;
 
			/* Move pointer to next character */
			ptrOutputCharHead = INC_CIRC_BUFFER_PTR(ptrOutputCharHead, 
													zOutputChars,
													MAX_OUT_STR_SIZE);
			++Str_P;
		}
		else
		{   /* Done copying characters */
			bCopyComplete = TRUE;
		}
	}   /* end of copying string */

	/* Enable transmitter interrupt */
	SET_BIT(UCSR0B, UDRIE0);

	/* See if the reason we stopped copying the string was because the 
	 * buffer was full. Note that the buffer being full may no longer be the
	 * case, even if it was true when copying, so we have to use our complete
	 * flag. */
	if (bCopyComplete == FALSE)
	{   /* Prematurely terminated copying */
		ReportError(SCI_TX_BUFFER_OVERFLOW);
		iReturnCode = -1;
	}

	return iReturnCode;
}

/******************************************************************************
 * Returns a character in the Receive buffer. If it's empty, it returns
 * a 0.
 ******************************************************************************/
char SCIReadChar(void)
{
	char cReturnVal;

	if (ptrInputCharHead != ptrInputCharTail)
	{   /* Receive buffer is not empty */
		cReturnVal = *ptrInputCharTail;

		/* Move pointer to next character */
		ptrInputCharTail = INC_CIRC_BUFFER_PTR(ptrInputCharTail, 
											   zInputChars,
											   MAX_IN_STR_SIZE);
	}
	else
	{
		cReturnVal = 0;
	}
	return cReturnVal;
}
