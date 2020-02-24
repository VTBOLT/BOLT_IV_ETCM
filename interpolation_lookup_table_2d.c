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
    float min_key = 0;
    float max_key = 0;
    for(int i = 0; i < LOOKUP_TABLE_2D_SIZE; i++) {
        float current = table.keys[i];
        min_key = fminf(current, min_key);
        max_key = fmaxf(current, max_key);
    }
    table.min_key = min_key;
    table.max_key = max_key;
}

float get(const lookup_table_2d_t& table, float key_value) {
    // Find approximate index
    float key_interpolation_scalar = get_interpolation_scalar(table.min_key, table.max_key, key_value);
    float approx_key_index = key_interpolation_scalar * (LOOKUP_TABLE_2D_SIZE - 1);
    // The approximate index will be a decimal value - so find its 2 integer neighbors
    int search_index_floor = floorf(approx_key_index);
    int search_index_ceil = ceilf(approx_key_index);

    bool found = false;

    while(!found) {
        // Check if value is between two values in the table
        if(key_value >= table.keys[search_index_floor] &&key_value <= table.values[search_index_ceil]) {
            found = true;
        // Since the lookup table is ordered, we can decide whether to iterate left or right
        } else if(key_value > table.keys[search_index_ceil]) {
            // If we're going out of bounds, just use the last value in the lookup table
            if(search_index_ceil + 1 >= LOOKUP_TABLE_2D_SIZE) {
                found = true;
            } else {
                search_index_floor++;
                search_index_ceil++;
            }
        } else if(key_value < table.keys[search_index_floor]) {
            if(search_index_floor - 1 <= 0) {
                found = true;
            } else {
                search_index_floor--;
                search_index_ceil--;
            }
        }
    }// Interpolate between the value from the lower index and the value from the higher index.
    float interpolated_value = interpolate(table.values[search_index_floor], table.values[search_index_ceil], );

    return interpolated_value;
}


