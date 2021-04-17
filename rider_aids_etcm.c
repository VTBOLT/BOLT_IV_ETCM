/*
 * rider_aids_etcm.c
 *
 *  Created on: Mar 7, 2020
 *      Author: willc
 */

#include <rider_aids_etcm.h>

bool checkTC(uint32_t frontWhlSpd, uint32_t backWhlSpd)
{
    if (frontWhlSpd > backWhlSpd) {
        if (pctDiff(frontWhlSpd, backWhlSpd) > 15) {
            return true;
        }
    }
    else if (backWhlSpd < frontWhlSpd) {
        if (pctDiff(backWhlSpd, frontWhlSpd) > 15) {
            return true;
        }
    }

    return false;
}

bool checkAntiJerk(float roll, int frontWhlSpd, int backWhlSpd)
{
    if (roll > 35 | roll < -35) { // check these values
        return true;
    }

    return false;
}

bool checkAntiWheelie(float pitch, float frontSusp, float backSusp)
{
    if (frontSusp > 125 | backSusp < 25) {
        return true;
    }

    if (pitch > 10) { // do some trig to find real angle
        return true;
    }

    return false;
    // maybe make int dep on res
}


// Helpers
uint32_t pctDiff(uint32_t larger, uint32_t smaller)
{
    float f_larger = (float)larger;
    float f_smaller = (float)smaller;
    float rawPct = ((f_larger - f_smaller) / ((f_larger + f_smaller) / 2));

    return (uint32_t)(rawPct * 100);
}
