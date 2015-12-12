/******************************************************************************
 * File Name:	Main.c
 * Program:		"Serial Echo" program for Fundamentals of Embedded Systems 
 * Author:		Robert Weber
 * Purpose:		Main file. Contains the Main function and other executive
 *				functions. It's the "master" of the program.
 *
 *  Date		Changed by	Changes:
 * ---------	----------	-------------------------------------------------------
 * 24 Apr 02	R Weber		Original file.
 * 12 Feb 04	T Lill		Modified for class project.
 * 17 Nov 04	T Lill		Replaced while (TRUE) with for ( ; ; )
 * 03 May 05	T Lill		Removed deprecated functions.
 * 15 Dec 06	T Lill		Updated hearbeat function.
 * 17 Oct 10	T Lill		Modified program for ATmega2560 processor.
 * 29 Apr 11	T Lill		Use port D for LEDs
 ******************************************************************************/
#include <avr/io.h>
#include "lib.h"
#include "interrpt.h"
#include "heartbeat.h"
#include "serial.h"

/************************* Function Prototypes ******************************/
int main(void);


/*********************** Function Implementations ******************************/

void heartbeat(void)
{
	asm volatile (" sbi	0x09, 0 ");	// see warning in lib.h about SET_BIT
}

/*****************************************************************************
 * Main function.
 * 
 * Initializes hardware and software. Then enters the endless foreground loop.
 * 
 *****************************************************************************/
int main(void) 
{
   char buffer[2];
   
    // Disable interrupts
    cli();

	// Set port D as an output and turn off all LEDs
	DDRD =0xFF;
	PORTD =0xFF;

    /* Initialize the Timer 0 */
    ISR_InitTimer0();

	// Initialize serial I/O
	SCIInitialize();
	
    /* Enable interrupts. Do as last initialization, so interrupts are
     * not initiated until all of initialization is complete. */
   sei();

   for ( ; ; )		/* Foreground loops forever */
   {   // Do slow tasks here

      // Check for a received character, and echo it back
      // to the "screen"
      if ((buffer[0] = SCIReadChar()) != 0)
      {  // Recived a character
         buffer[1] = '\0';
         SCIWriteString(buffer);
      }
      
   }   /* end of endless loop */

   return 0;
}
