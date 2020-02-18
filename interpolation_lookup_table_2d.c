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
    int approx_index_floor = floorf(approx_index);
    int approx_index_ceil = ceilf(approx_index);

    // Store the indices we're searching separately;
    // We need the original indices as a reference so we can search left and right simultaneously
    int search_index_floor = approx_index_floor;
    int search_index_ceil = approx_index_ceil;
    bool found = false;

    while(!found) {
        if(value >= table.values[search_index_floor] && value <= table.values[search_index_ceil]) {
            found = true;
        } else if(value > table.values[search_index_floor]) {
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
    approx_index = (value - table.values[search_index_floor]) / (table.values[search_index_ceil] - table.values[search_index_floor]) * (LOOKUP_TABLE_2D_SIZE - 1);
    // Interpolate between the value from the lower index and the value from the higher index.
    float interpolated_value = interpolate(table.values[search_index_floor], table.values[search_index_ceil], approx_index / (search_index_ceil - search_index_floor));

    return interpolated_value;
}


