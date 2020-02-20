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
    int search_index_floor = floorf(approx_index);
    int search_index_ceil = ceilf(approx_index);

    bool found = false;

    while(!found) {
        // Check if value is between two values in the table
        if(value >= table.values[search_index_floor] && value <= table.values[search_index_ceil]) {
            found = true;
        // Since the lookup table is ordered, we can decide whether to iterate left or right
        } else if(value > table.values[search_index_floor]) {
            // If we're going out of bounds, just use the last value in the lookup table
            if(search_index_ceil + 1 >= LOOKUP_TABLE_2D_SIZE) {
                found = true;
            } else {
                search_index_floor++;
                search_index_ceil++;
            }
        } else if(value < table.values[search_index_ceil]) {
            if(search_index_floor - 1 <= 0) {
                found = true;
            } else {
                search_index_floor--;
                search_index_ceil--;
            }
        }
    }
    // Recalculate the approximate index by interpolating by two nearest values
    approx_index = (value - table.values[search_index_floor]) / (table.values[search_index_ceil] - table.values[search_index_floor]) * (LOOKUP_TABLE_2D_SIZE - 1);
    // Interpolate between the value from the lower index and the value from the higher index.
    float interpolated_value = interpolate(table.values[search_index_floor], table.values[search_index_ceil], approx_index / (search_index_ceil - search_index_floor));

    return interpolated_value;
}


