
//#include "../../driverlib/*"
//#include "inc/hw_types.h"
//#include "inc/hw_gpio.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_sysctl.h"
#include "stdint.h"
#include "stdbool.h"
#include "../../driverlib/gpio.h"
#include "../../driverlib/rom.h"
#include "../../driverlib/sysctl.h"
#include "../../driverlib/pin_map.h"
#include "../../driverlib/can.h"
/**
 * main.c
 */

extern int fib(int n);

int main(void){
    //int fib_number = fib(5);
	return 0;
}

void blink(char n) {
/*    //n = pattern for me
    for(int i =0; i < NUM; i++){
        int state = ANOTHER_NUM;
        ___SET__LED__STATE___
        ___WAIT between digits____
    }
    ___long wait between numbers___
//testing new branch. That's all.*/
}

//come back to type. Will probably pass back array.
 //Depends on blink impl.
void morse_conversion(int n){
    //takes in number, converts to array of bases.
    int mod;
    while(n != 0){
        mod = n % 10;
        blink(mod);
        n = (n - mod)/10;
    }
}

void package_blink(char n){
    char coded_n = 0b11111;
    if(n <= 5){
        coded_n << (n-5);
    }
    else{
        coded_n >> n;
    }
    blink(coded_n);
}
