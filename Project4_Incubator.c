/* 
 * File: ECE_218_Project4_Incubator
 * Author: David Koval, Sirus Negahvan, Vincent Yang
 * Date: Wednesday, 7 March 2018
 */
/*********** COMPILER DIRECTIVES *********/

// #include for textbook library header files
#include "pic24_all.h"
#include "lcd4bit_lib.h"

// #defines for handy constants
#define fan LATAbits.LATA4
/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/
float Temperature;  // Temperature input from sensor
float Humidity;     // Humidity input from sensor
char temp[];        // LCD array for temperature
char humd[];        // LCD array for humidity

// Given a temperature floating point and humidity floating point
// This function displays those two values to the LCD screen.
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
	configClock();      //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat();  //Blinks the LED on RA1
    configControlLCD(); //configures RS, RW, and E control lines as outputs and initializes them low.
    initLCD();          //executes the initialization sequence specified in teh Hitachi HD44780 LCD controller datasheet,
    _TRISA4 = 0;                    //clears the screen and sets the cursor position to upper left (home).
    

    
/* Initialize ports and other one-time code */
    Temperature = 98.6;
    Humidity = 60.1;
    
/* Main program loop */
	while (1) {	
        doHeartbeat();
        
        writeLCD(0b00011100, 0, 1, 1);
        displayStats(Temperature, Humidity);
        
        DELAY_MS(600);
		}
}
