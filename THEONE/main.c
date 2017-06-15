
/*
 * main.c
 */
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/can.h"
#include "driverlib/timer.h"

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
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }


    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);


    //int fib_number = fib(5);
    int i;
    for(i = 11; i < 20; i++){
        morse_conversion(i);
    }

    return 0;
}

void blink(char n) {
    char mask = 0b10000;    //change to char if not working
    volatile uint32_t ui32Loop;

    uint8_t i;
    for(i = 0; i < 5; i++){
        mask = 0b10000;
        //
        // Turn on the LED.
        //
        mask = mask & n;
        n = n << 1;
        if (mask == 0b00000){  //1 corresponds to a dash, 0 is dot
            GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, BLUE_LED);
            for(ui32Loop = 0; ui32Loop < 400000; ui32Loop++){}
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GREEN_LED);
            for(ui32Loop = 0; ui32Loop < 600000; ui32Loop++){}
        }

        //
        // Turn off the LED.
        //
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED, 0x0);

        //
        // Delay for a bit.
        //
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}
    }
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);
    for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x0);
    for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}
    if(flag){
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);
        for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){}
    }
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x0);
    for(ui32Loop = 0; ui32Loop < 1000000; ui32Loop++){}
}

//come back to type. Will probably pass back array.
 //Depends on blink impl.
void morse_conversion(int n){
    //takes in number, converts to array of bases.
    flag = 0;   //flag is 1 when loop is complete. All numbers packaged. (i.e. 123 -> 1, 2, 3)
    int mod;
    while(n != 0){
        mod = n % 10;
        package_blink(mod); //note that this code might overwrite itself as one runs full-tilt and the morse output is timed.
        n = (n - mod)/10;
    }
    flag = 1;
}

void package_blink(char n){
    /*  coded_n = 0b11111;
        if(n <= 5)
        coded_n = coded_n << (n-5);
    else
        coded_n = coded_n >> n;
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
