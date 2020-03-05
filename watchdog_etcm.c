/*
 * watchdog.c
 *
 *  Created on: Mar 4, 2020
 *      Author: Tyler
 *
 *  t_WDL:  680-920ms (800ms typical)
 *  t_WDU:  1360-1840ms (1600 typical)
 *
 *  Must pulse WDT at least every 1000ms. This is within the window.
 *  The WDT resets on a falling edge.
 *
 *  This WDT is windowed. See TPS3430 datasheet.
 *
 */

#include <watchdog_etcm.h>

void initWatchdog(){
    GPIO_setPinConfig(GPIO_CFG_WATCHDOG);
    GPIO_setPadConfig(GPIO_WATCHDOG, GPIO_PIN_TYPE_STD);        // Push/pull
    GPIO_setDirectionMode(GPIO_WATCHDOG, GPIO_DIR_MODE_OUT);

    //resetWatchdog();
}

/**
 * Call at start of protected code.
 */
void resetWatchdog(){
    // Reset (falling edge)
    GPIO_writePin(GPIO_WATCHDOG, 1);
    DEVICE_DELAY_US(1);
    GPIO_writePin(GPIO_WATCHDOG, 0);
}


