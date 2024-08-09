/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 * 7-1 Project Submission
 * Kyle Dale
 * 10/17/2023
 * CS350-Emerging System Architecture & Tech
 * Professor Pettit
 * gpiointerrupt.c
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/I2C.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define DISPLAY(x) UART2_write(uart, &output, x, NULL);

// I2C Global Variables
static const struct {
    uint8_t address;
    uint8_t resultReg;
    char *id;
} sensors[3] = {
    { 0x48, 0x0000, "11X" },
    { 0x49, 0x0000, "116" },
    { 0x41, 0x0001, "006" }
};
uint8_t txBuffer[1];
uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;
I2C_Handle i2c;

// UART Global Variables
char          output[64];
int           bytesToSend;
// Driver Handles - Global variables
UART2_Handle uart;

// Driver Handles - Global variables
Timer_Handle timer0;

volatile unsigned char TimerFlag1s = 0;
volatile unsigned char TimerFlag500ms = 0;
volatile unsigned char TimerFlag200ms = 0;

// Counter for the timer callback
volatile unsigned int timerCounter = 0;

void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    timerCounter++;

    if (timerCounter % 5 == 0) {
        TimerFlag1s = 1;
    }

    if (timerCounter % 2 == 0) {
        TimerFlag500ms = 1;
    }

    if (timerCounter == 1 || timerCounter % 2 == 1) {
        TimerFlag200ms = 1;
    }
}

void initTimer(void) {
    Timer_Params params;

    // Init the driver
    Timer_init();

    // Configure the driver
    Timer_Params_init(&params);
    params.period = 200000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    // Open the driver
    timer0 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer0 == NULL) {
        /* Failed to initialize timer */
        while (1) {}
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        while (1) {}
    }
}


void initUART(void) {
    UART2_Params uartParams;

    // Init the driver
    //UART_init();

    // Configure the driver
    UART2_Params_init(&uartParams);
    //uartParams.writeDataMode = UART_DATA_BINARY;
    //uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART2_ReadReturnMode_FULL;
    uartParams.baudRate = 115200;

    // Open the driver
    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }
}


// Make sure you call initUART() before calling this function.
void initI2C(void)
{
    int8_t i, found;
    I2C_Params i2cParams;
    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "))

    // Init the driver
    I2C_init();

    // Configure the driver
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    // Open the driver
    i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL)
    {
        DISPLAY(snprintf(output, 64, "Failed\n\r"))
        while (1);
    }
    DISPLAY(snprintf(output, 32, "Passed\n\r"))

    // Boards were shipped with different sensors.
    // Welcome to the world of embedded systems.
    // Try to determine which sensor we have.
    // Scan through the possible sensor addresses

    /* Common I2C transaction setup */
    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = 0;
    found = false;
    for (i = 0; i < 3; ++i)
    {
        i2cTransaction.targetAddress = sensors[i].address;
        txBuffer[0] = sensors[i].resultReg;
        DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id))
        if (I2C_transfer(i2c, &i2cTransaction))
        {
            DISPLAY(snprintf(output, 64, "Found\n\r"))
            found = true;
            break;
        }
        DISPLAY(snprintf(output, 64, "No\n\r"))
    }
    if (found)
    {
        DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address: %x\n\r", sensors[i].id, i2cTransaction.targetAddress))
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Temperature sensor not found, contact professor\n\r"))
    }
}

int16_t temperature = 0;
// Read Temp function
int16_t readTemp(void)
{
    i2cTransaction.readCount = 2;
    if (I2C_transfer(i2c, &i2cTransaction))
    {
        /*
        * Extract degrees C from the received data;
        * see TMP sensor datasheet
        */
        temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
        temperature *= 0.0078125;
        /*
        * If the MSB is set '1', then we have a 2's complement
        * negative value which needs to be sign extended
        */
        if (rxBuffer[0] & 0x80)
        {
            temperature |= 0xF000;
        }
    }
    else
    {
        DISPLAY(snprintf(output, 64, "Error reading temperature sensor (%d)\n\r", i2cTransaction.status))
        DISPLAY(snprintf(output, 64, "Please power cycle your board by unplugging USB and plugging back in.\n\r"))
    }
    return temperature;
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
volatile unsigned char buttonPressed = 0;

void gpioButtonFxn0(uint_least8_t index)
{
    buttonPressed = 1; // Set the button press flag

    // FOR TESTING PURPOSES
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
}


/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */

/* REMOVED TO AVOID CONFLICT
void gpioButtonFxn1(uint_least8_t index)
{
    Toggle an LED
    //GPIO_toggle(CONFIG_GPIO_LED_1);
}
*/


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();
    initUART();
    initI2C();
    initTimer();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    //GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    //if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1)
    //{
        /* Configure BUTTON1 pin */
        //GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

        /* Install Button callback */
        //GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        //GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    //}

    int16_t setPoint = 22; // Set-point temperature
    int heatOn = 0; // Variable to track whether the heat is on (0 for off, 1 for on)
    int secondsSinceReset = 0; // Variable to track seconds since board reset
    int16_t currentTemperature = 0;
    int x;

    // TASK SCHEDULER
    while (1) {
        if (TimerFlag200ms) {
            // Check if the button was pressed during the 200ms interval
            if (buttonPressed) {
                x++; // Increment temperature by one degree
                buttonPressed = 0; // Reset the button press flag
            }
            TimerFlag200ms = 0; // Reset the flag
        }

        if (TimerFlag500ms) {
            // Call readTemp() at the 500ms interval
            int16_t currentTemperature = readTemp();
            // Use the currentTemperature as needed
            TimerFlag500ms = 0; // Reset the flag
        }

        if (TimerFlag1s) {
            // Increment seconds since reset
            secondsSinceReset++;

            // Check if the current temperature is higher than the set-point
            if ((currentTemperature + x) > setPoint) {
                // Turn off the heat by turning off the LED
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
                heatOn = 0; // Heat is off
            } else {
                // Turn on the heat by turning on the LED
                GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
                heatOn = 1; // Heat is on
            }

            // Format and send data via UART2
            char uartOutput[64];
            snprintf(uartOutput, sizeof(uartOutput), "<%02d,%02d,%d,%04d>", currentTemperature, setPoint, heatOn, secondsSinceReset);
            UART2_write(uart, uartOutput, strlen(uartOutput), NULL);

            TimerFlag1s = 0; // Reset the flag
        }
    }
    return (NULL);
}

