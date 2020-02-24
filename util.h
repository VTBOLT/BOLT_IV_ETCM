/*
 * util.h
 *
 *  Created on: Feb 17, 2020
 *      Author: Stephen Welch
 */

#ifndef UTIL_H_
#define UTIL_H_

/**
 * Interpolates between a start value and end value based on an amount from 0-1.
 */
float interpolate(float start, float end, float amount) {
    return start + ((end - start) * amount);
}

/**
 * Returns the percentage amount that a given value is between two other values.
 */
float get_interpolation_scalar(float start, float end, float amount) {
    return (amount - start) / (end - start);
}

/**
 * Checks if a and b are within a amount of each other.
 */
bool epsilon_equals(float eps, float a, float b) {
    if(a - b < eps || b - a < eps) {
        return true;
    } else {
        return false;
    }
}

#endif /* UTIL_H_ */
