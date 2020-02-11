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
    float approx_index = (value - table.min_value) / (table.max_value - table.min_value) * (LOOKUP_TABLE_2D_SIZE - 1);
    // The approximate index will be a decimal value - so find its 2 integer neighbors
    int index_floor = floorf(approx_index);
    int index_ceil = ceilf(approx_index);

    // Interpolate between the value from the lower index and the value from the higher index.
    float interpolated_value = interpolate(table.values[index_floor], table.values[index_ceil], approx_index / (index_ceil - index_floor));

    return interpolated_value;
}


