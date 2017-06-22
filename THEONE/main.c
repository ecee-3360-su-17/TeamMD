
/*
 * main.c
 */
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

extern int fib(int n);
void blink(char);
void morse_conversion(int);
void package_blink(char);

bool flag = 0;

int main(void){

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}

    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);

    int fib_n;
    fib_n = fib(7);
    morse_conversion(fib_n);
    /*int i;
    for(i = 11; i < 20; i++){
        morse_conversion(i);
        //morse_conversion(fib(i));
    }*/

    while(1){}  //Just keep going, buddy.
    return 0;
}

void blink(char n) {
    char mask = 0b10000;    //change to char if not working
    volatile uint32_t ui32Loop;

    uint8_t i;
    for(i = 0; i < 5; i++){
        //char is read left-to-right (as all things should be)
        //So start the bit mask on the left-most bit.
        mask = 0b10000;
        mask = mask & n;        //Mask out
        n = n << 1;             //Shift the packaged char because left shift always 0-fills
                                //avoids the issue of compiler-specific right shifts and uninitialized data. (even though mask is initialized)
        if (mask == 0b00000){  //1 corresponds to a dash (green), 0 is dot (blue)
            GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, BLUE_LED);
            for(ui32Loop = 0; ui32Loop < 400000; ui32Loop++){}  //ghetto-delay loop
        }
        else{   //it's a 0b10000. (dash)
            GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GREEN_LED);
            for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}  //ghetto-delay loop
        }

        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED, 0x0); //turn off LEDs
        for(ui32Loop = 0; ui32Loop < 600000; ui32Loop++){}      //delay between blinks

    }
    //Single red is for the end of a single number having finished it's morse-output
    //i.e. 1 or the 3 from 32, etc.
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);    //turn on LED
    for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x0);        //turn off LED
    for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}  //ghetto-delay
    //Checks if the entire number has been displayed.
    //i.e. 3 AND 2 from 32.
    if(flag){   //flag is set in morse_conversion() if all of a number has been packaged and sent off
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);
        for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}  //ghetto-delay
    }
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x0);
    for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}     //ghetto-delay
}

//Takes in a int (conveniently the one passed back from fib().)
//converts it to single-digit numbers as chars
//sends it to package_blink() to convert to a morse-code char
void morse_conversion(int n){
    //takes in number, converts to array of bases.
    flag = 0;   //flag is 1 when loop is complete. All numbers packaged. (i.e. 123 -> 1, 2, 3)
    char mod;
    char mod_array[6] = {0,0,0,0,0,0};
    int i = 0;
    while(n != 0){
        mod = n % 10;
        mod_array[i] = mod;
        i++;
        n = (n - mod)/10;
    }
    while(i > 0){   //ghetto and sad offset loop.
        flag = i == 1 ? 1 : 0;
        package_blink(mod_array[i-1]);
        i--;
    }
}

//converts numbers to 5-bit (technically in chars) coded morse representations
//Sends result off to blink() for actual LED output in morse code
void package_blink(char n){
    //Abuses 0-fill to shift in the associated amount of 0's for morse output.
/*  char coded_n = 0b011111;
    if(n <= 5){
        coded_n = coded_n << (5-n);
    }
    else{
        coded_n = coded_n >> n;
    }
    blink(coded_n);*/
    switch(n){
    case 0:
        n = 0b11111;
            break;
        case 1:
            n = 0b01111;
            break;
        case 2:
            n = 0b00111;
            break;
        case 3:
            n = 0b00011;
            break;
        case 4:
            n = 0b00001;
            break;
        case 5:
            n = 0b00000;
            break;
        case 6:
            n = 0b10000;
            break;
        case 7:
            n = 0b11000;
            break;
        case 8:
            n = 0b11100;
            break;
        case 9:
            n = 0b11110;
            break;
        default:
            n = 0b10101;
            break;
        }
        blink(n);
}
