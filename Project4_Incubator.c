/* 
 * File:   
 * Author: 
 * Date:
 * Purpose:
 * Modified:
 */
/*********** COMPILER DIRECTIVES *********/

// #include for textbook library header files
#include "pic24_all.h"
#include "lcd4bit_lib.h"

// #defines for handy constants 
#define LED LATAbits.LATA0  // LED on microstick is connected to RA0 (PORTA, bit 0)


/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/
float Temperature;
float Humidity;
char temp[];
char humd[];

void displayStats(float temperature, float humidity){
    writeLCD(0x84, 0, 1, 1);
    sprintf(temp, "%3.1f",temperature);
    outStringLCD("Temp: ");
    outStringLCD(temp);
    outStringLCD("F");
    
    writeLCD(0xC0, 0, 1, 1);
    sprintf(humd, "%3.1f", humidity);
    outStringLCD("Humidity: ");
    outStringLCD(humd);
    outStringLCD("%");
}
    
/********** MAIN PROGRAM LOOP********************************/
int main ( void )  //main function that....
{ 
/* Define local variables */


/* Call configuration routines */
	configClock();  //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat(); //Blinks the LED on RA1
    configControlLCD();    //configures RS, RW, and E control lines as outputs and initializes them low.
    initLCD();  //executes the initialization sequence specified in teh Hitachi HD44780 LCD controller datasheet,
                //clears the screen and sets the cursor position to upper left (home).

    
/* Initialize ports and other one-time code */
    Temperature = 98.6;
    Humidity = 60.1;
    //write string hello world!
    
    
    
/* Main program loop */
	while (1) {	
        doHeartbeat();
        
        writeLCD(0b00011100, 0, 1, 1);
        displayStats(Temperature, Humidity);
        
        DELAY_MS(600);
		}
}
