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
float interpolate(float start, float end, float amount);

/**
 * Checks if a and b are within a amount of each other.
 */
bool epsilon_equals(float eps, float a, float b);

#endif /* UTIL_H_ */
