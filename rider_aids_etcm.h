/*
 * rider_aids_etcm.h
 *
 *  Created on: Feb 26, 2020
 *      Author: Will Campbell
 */

#include <stdint.h> // why is this necessary here but not other .c/.h modules?
#include <stdbool.h>

#define MAX_WHEEL_RPM 459 // see ecap_etcm.c for ranges


#ifndef RIDER_AIDS_ETCM_H_
#define RIDER_AIDS_ETCM_H_

bool checkTC(uint32_t frontWhlSpd, uint32_t backWhlSpd);
bool checkAntiJerk(float roll, int frontWhlSpd, int backWhlSpd);
bool checkAntiWheelie(float pitch, float frontSusp, float backSusp); // maybe make int dep on res


// Helpers
uint32_t pctDiff(uint32_t larger, uint32_t smaller);


#endif /* RIDER_AIDS_ETCM_H_ */
