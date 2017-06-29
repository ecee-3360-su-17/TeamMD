/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uartecho.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Mailbox.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

/* Include GPIO and system shtuff */
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#define NUMMSGS 3   //defines number of messages in mailbox
#define TASKSTACKSIZE     768
#define BUFFER_SIZE 10
int buffer_count = 0;
char buffer[BUFFER_SIZE];

UART_Handle uart;           //attempt to not have a task for just managing the UART

// Task structures
Task_Struct taskTxStruct, taskRxStruct;
Char taskTxStack[TASKSTACKSIZE];
Char taskRxStack[TASKSTACKSIZE];



typedef struct MsgObj{
    int val;
} MsgObj, *Msg; //created mailbox object type

void clear_buffer(){
    //massively ghetto clear
    int iter;
    for (iter = 0; iter < BUFFER_SIZE; iter++){
        buffer[iter] = 0;
    }
}

void start_uart(){
    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }
}

void check_buffer(){
    //
    char test_string[BUFFER_SIZE];
    strcpy(test_string, "on");
    if (strcmp(buffer, test_string) == 0) {
        //turn on
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED|RED_LED, BLUE_LED|GREEN_LED|RED_LED);  //Turn on all LEDs -> white
        clear_buffer();
        return;
    }
    strcpy(test_string, "off");
    if (strcmp(buffer, test_string) == 0){
        //turn off
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED|RED_LED, 0x0); //Turn off all LEDs
        clear_buffer();
        return;
    }
    strcpy(test_string, "red");
    if (strcmp(buffer, test_string) == 0){
        //do red things
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED|RED_LED, 0x0); //Turn off all LEDs
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);  //Turn on red LED
        clear_buffer();
        return;
    }
    strcpy(test_string, "blue");
    if (strcmp(buffer, test_string) == 0){
        //do blue things
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED|RED_LED, 0x0); //Turn off all LEDs
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, BLUE_LED);  //Turn on red LED
        clear_buffer();
        return;
    }
    strcpy(test_string, "green");
    if (strcmp(buffer, test_string) == 0){
        //do green things
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED|GREEN_LED|RED_LED, 0x0); //Turn off all LEDs
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GREEN_LED);  //Turn on red LED
        clear_buffer();
        return;
    }

    const char echoPrompt[] = "\fInvalid input";

    UART_write(uart, echoPrompt, sizeof(echoPrompt));
    clear_buffer(); //else clear buffer -> throw incorrect input
}

/*
 * Remember to plug Rx into Tx, kids.
 */
void senderFxn(UArg arg0, UArg arg1){
    Mailbox_Handle mbxHandle = (Mailbox_Handle)arg0;
    int mbxCount = 0;

    const char echoPrompt[] = "\fEnter <on|off|red|blue|green>:\r\n";

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    while(1){
        MsgObj msg;
        UART_read(uart, &msg.val, 1);
        UART_write(uart, &msg.val, 1);
        bool test = Mailbox_post(mbxHandle, &msg, BIOS_WAIT_FOREVER);   //that's a long time.
        mbxCount++;
    }
}

void receiverFxn(UArg arg0, UArg arg1){
    Mailbox_Handle mbxHandle = (Mailbox_Handle)arg0;

    const char echoPrompt[] = "\f\r\n";

    while(1){
        MsgObj msg;
        Mailbox_pend(mbxHandle, &msg, BIOS_WAIT_FOREVER);   //Still a really long time. Props, kernel.
        if (msg.val == '\n' || (char)msg.val == 0x0d || buffer_count == BUFFER_SIZE){
            check_buffer();
            buffer_count = 0;   //reset locally
            UART_write(uart, echoPrompt, sizeof(echoPrompt));
        }
        else {
            buffer[buffer_count] = msg.val;
            buffer_count++;
        }
    }
}

/*
 *  ======== main ========
 */
int main(void){
    // Call board init functions
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

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

    // AOL setup
    Mailbox_Struct mbxStruct;

    Mailbox_Params mbxParams;
    Mailbox_Params_init(&mbxParams);

    Mailbox_construct(&mbxStruct, sizeof(MsgObj), 20, &mbxParams, NULL);
    Mailbox_Handle mbxHandle = Mailbox_handle(&mbxStruct);

    // Construct BIOS objects

    //Receiver parameters
    Task_Params taskRxParams;
    Task_Params_init(&taskRxParams);
    taskRxParams.stackSize = TASKSTACKSIZE;
    taskRxParams.arg0 = (UArg)mbxHandle;
    taskRxParams.stack = &taskRxStack;
    taskRxParams.instance->name = "receive";
    Task_construct(&taskRxStruct, (Task_FuncPtr)receiverFxn, &taskRxParams, NULL); //Construct Rx Tasks


    //sender parameters
    Task_Params taskTxParams;
    Task_Params_init(&taskTxParams);
    taskTxParams.stackSize = TASKSTACKSIZE;
    taskTxParams.arg0 = (UArg)mbxHandle;
    taskTxParams.stack = &taskTxStack;
    taskTxParams.instance->name = "send";   //look this dealio up maybe kinda. Probably self-explanatory but...
    Task_construct(&taskTxStruct, (Task_FuncPtr)senderFxn, &taskTxParams, NULL);   //Construct Tx task

    start_uart();

    // Start BIOS
    BIOS_start();

    return (0);
}
