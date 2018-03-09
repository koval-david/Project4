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
#define PWM_PERIOD 3125         //20ms PWM period with clock cycle of 6.4us from 1:256 prescale
#define LIGHT_MIN 203           //1.3ms (min) pulse width for Lamp servo
#define EGG_ROLLER_MIN 117      //0.75ms (min) pulse width for Egg Roller servo
#define STOP 234
#define One_Rotation 200        //200ms delay for one rotation

#define ledRed LATAbits.LATA2   //RA2 controls red led
#define ledGreen LATAbits.LATA3 //RA3 controls green led
#define fan LATAbits.LATA4      //RA4 is controls fan

#define I2C_ADD 0x80
#define temp_size 3
#define hum_size 3

/*********** GLOBAL VARIABLE AND FUNCTION DEFINITIONS *******/
float Temperature;              //Temperature input from sensor
float Humidity;                 //Humidity input from sensor
char tempINT[];                 //LCD array for temperature
char humdINT[];                 //LCD array for humidity
uint8_t temp[temp_size];
uint8_t hum[hum_size];
uint16_t LIGHT_pulse_width;     //Pulse width for Lamp servo
uint16_t EGG_ROLLER_pulse_width;//Pulse width for Egg Roller servo
int Light_ON = 0;               //Status of Lamp

// Configures Output compare module 1 for continuous Rotation Servo - Light
void configOC1() {
    T2CONbits.TON = 0;        //Turns timer 2 off.
    CONFIG_OC1_TO_RP(RB0_RP); //Maps the OC1 output to the remappable pin, RB0.
    OC1RS = 0;                //Clears the RS register
    OC1R = 0;                 //Clears the R register
    OC1CONbits.OCTSEL = 0;    //Sets the output compare module to use Timer 2 as the clock source.
    OC1CONbits.OCM = 0b110;   //Sets it to operate in PWM mode with fault pin disabled.
}

// Configures Output compare module 2 for continuous Rotation Servo - Egg Roller
void config0C2(void){
    T2CONbits.TON = 0;          //Turns timer 2 off.
    CONFIG_OC2_TO_RP(RB1_RP);   //Maps the OC2 output to the remappable pin, RB1.
    OC2RS = 0;                  //Clears the OC2RS register
    OC2R = 0;                   //Clears the OC2R register
    OC2CONbits.OCTSEL = 0;      //Sets the output compare module to use Timer 2 as the clock source.
    OC2CONbits.OCM = 0b110;     //Sets it to operate in PWM mode with fault pin disabled.
}

//Configures Timer 2
void configTimer2() {
    T2CON = 0x0030;             //Configs timer 2, with presacle 1:256
    PR2 = PWM_PERIOD;           //Sets period
    TMR2 = 0;                   //Clears the timer 2 register.
    _T2IF = 0;                  //Clears the flag, T2IF
}

//Configures the timer 2 interupt and loads its registers
void _ISR _T2Interrupt(void) {
    OC1RS = LIGHT_pulse_width;          //Load OC1RS register with Lamp pulse width
    OC2RS = EGG_ROLLER_pulse_width;     //Load OC2RS register with Egg Roller pulse width
    _T2IF = 0;                          //Clears the Timer 2 interrupt flag, the last instruction in ISR
}

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

// Given a temperature floating point and humidity floating point
// This function displays those two values to the LCD screen.
void displayStats(float temperature, float humidity){
    writeLCD(0x84, 0, 1, 1);
    sprintf(tempINT, "%3.1f",temperature);
    outStringLCD("Temp: ");
    outStringLCD(tempINT);
    outStringLCD("F");
    
    writeLCD(0xC0, 0, 1, 1);
    sprintf(humdINT, "%3.1f", humidity);
    outStringLCD("Humidity: ");
    outStringLCD(humdINT);
    outStringLCD("%");
}
    
/********** MAIN PROGRAM LOOP********************************/
int main ( void )  //main function that....
{ 
/* Define local variables */

/* Call configuration routines */
	configClock();          //Sets the clock to 40MHz using FRC and PLL
  	configHeartbeat();      //Blinks the LED on RA1
    
    configOC1();            //Configures output compare module 1 for Lamp motor
    config0C2();            //Configures output compare module 2 for Egg Roller motor
    configTimer2();         //Configures timer 2
    
    configI2C1(400);        //Configures I2C to 400
    
    configControlLCD();     //configures RS, RW, and E control lines as outputs and initializes them low.
    initLCD();              //executes the initialization sequence specified in the Hitachi HD44780 LCD controller
                            //clears the screen and sets the cursor position to upper left (home).
    _TRISA2 = 0;            //Red Led
    _TRISA3 = 0;            //Gree Led
    _TRISA4 = 0;            //Fan

    
/* Initialize ports and other one-time code */
    _T2IE = 1;                      //Enables the timer 2 interrupts
    T2CONbits.TON = 1;              //Turns timer 2 on
    Light_ON = 0;
    LIGHT_pulse_width = 0;          //Light servo
    EGG_ROLLER_pulse_width = 0;     //Egg Roller servo
    
/* Main program loop */
	while (1) {
        
        Temperature = getTemp();
        Humidity = getHum();
        
        //Turns ON/OFF Lamp
        if(Temperature < 55.0 && Light_ON == 0){
            LIGHT_pulse_width = LIGHT_MIN;
            DELAY_MS(One_Rotation);
            LIGHT_pulse_width = 0;
            Light_ON = 1;
        }
        if(Temperature > 55.1 && Light_ON == 1){
            LIGHT_pulse_width = LIGHT_MIN;
            DELAY_MS(One_Rotation);
            LIGHT_pulse_width = 0;
            Light_ON = 0;
        }
        
        //Controls RED and GREEN LED
        if (Temperature > 55.0 && Temperature < 55.1 && Humidity > 43.0 && Humidity < 45.0){
            ledGreen = 1;
            ledRed = 0;
        }
        else{
            ledGreen = 0;
            ledRed = 1;
        }
        
        //Displays Temperature and Humidity to LCD
        //writeLCD(0b00011100, 0, 1, 1);
        displayStats(Temperature, Humidity);
        
        DELAY_MS(600);
		}
}
