/**
 * lookup_table_2d.h
 *
 *  Created on: Feb 10, 2019
 *      Author: Stephen Welch
 */

#ifndef LOOKUP_TABLE_2D_H_
#define LOOKUP_TABLE_2D_H_

#include "util.h"

#define LOOKUP_TABLE_2D_SIZE 10

typedef struct {
    float keys[LOOKUP_TABLE_2D_SIZE];
    float values[LOOKUP_TABLE_2D_SIZE];
    float min_value, max_value;
} lookup_table_2d_t;

/**
 * Performs any initialization needed to perform lookups. Typically this will
 * entail finding the largest and smallest values in the table.
 */
void init(const lookup_table_2d_t& table);
float get(const lookup_table_2d_t& table, float value);

#endif
