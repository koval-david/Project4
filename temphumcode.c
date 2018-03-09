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

// #defines for handy constants 
#define I2C_ADD 0x80
#define temp_size 3
#define hum_size 3
/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/

uint8_t temp[temp_size];
uint8_t hum[hum_size];

float getTemp(void) {
    write1I2C1(I2C_ADD, 0xE3);
    readNI2C1(I2C_ADD, temp, temp_size);
    
    uint16_t number = (uint16_t)temp[0] << 8 | temp[1];
    float temperature = ((((175.72*number)/65536)-46.85)*(9/5))+32;
    
    return temperature;
}

float getHum(void) {
    write1I2C1(I2C_ADD, 0xE5);
    readNI2C1(I2C_ADD, hum, hum_size);
    
    uint16_t number = (uint16_t)temp[0] << 8 | temp[1];
    float humidity = ((125.0*number)/65536.0)-6.0;
    
    return humidity;
}

/********** MAIN PROGRAM LOOP********************************/
int main ( void )  //main function that....
{ 
/* Define local variables */
    float curTemp;
    float curHum;

/* Call configuration routines */
	configClock();  //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat(); //Blinks the LED on RA1
    configI2C1(400);

/* Initialize ports and other one-time code */   
 
    curTemp = getTemp();
    curHum = getHum();
    
    while (1) {
        
    } 
}