/*
 * interpolation_lookup_table.c
 *
 *  Created on: Feb 10, 2020
 *      Author: Stephen Welch
 */

#include "lookup_table_2d.h"
#include "fpu_math.h"
#include "math.h"

void init(const lookup_table_2d_t& table) {
    float min_value = 0;
    float max_value = 0;
    for(int i = 0; i < LOOKUP_TABLE_2D_SIZE; i++) {
        float current = table.values[i];
        min_value = fminf(current, min_value);
        max_value = fmaxf(current, max_value);
    }
    table.min_value = min_value;
    table.max_value = max_value;
}

float get(const lookup_table_2d_t& table, float value) {
    // Find approximate index
    // If approximate index isn't actual location, linear search
    // Return value
}


