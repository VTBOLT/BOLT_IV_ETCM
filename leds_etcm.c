/*
 * leds_etcm.c
 *
 *  Created on: Jan 21, 2020
 *      Author: Tyler
 */

#include <leds_etcm.h>

void initLEDS(void){
    // RED_LED
    GPIO_setPinConfig(GPIO_CFG_RED_LED);
    GPIO_setPadConfig(GPIO_RED_LED, GPIO_PIN_TYPE_STD);        // Push/pull
    GPIO_setDirectionMode(GPIO_RED_LED, GPIO_DIR_MODE_OUT);

    // BLUE_LED
    GPIO_setPinConfig(GPIO_CFG_BLUE_LED);
    GPIO_setPadConfig(GPIO_BLUE_LED, GPIO_PIN_TYPE_STD);        // Push/pull
    GPIO_setDirectionMode(GPIO_BLUE_LED, GPIO_DIR_MODE_OUT);


    // ETCM STATUS
    GPIO_setPinConfig(GPIO_CFG_STATUS_LED);
    GPIO_setPadConfig(GPIO_STATUS_LED, GPIO_PIN_TYPE_STD);        // Push/pull
    GPIO_setDirectionMode(GPIO_STATUS_LED, GPIO_DIR_MODE_OUT);
}

void toggleRedLED(){
    static uint8_t toggleBit = 1;
    GPIO_writePin(GPIO_RED_LED, (toggleBit&0x01));
    toggleBit = ~toggleBit;
}

void toggleStatusLED(){
    static uint8_t toggleBit = 1;
    GPIO_writePin(GPIO_STATUS_LED, (toggleBit & 0x01));
    toggleBit = ~toggleBit;
}
