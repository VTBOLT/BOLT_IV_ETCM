/**
 * lookup_table_3d.h
 *
 *  Created on: Feb 10, 2019
 *      Author: Stephen Welch
 */

#ifndef LOOKUP_TABLE_3D_H_
#define LOOKUP_TABLE_3D_H_

#define LOOKUP_TABLE_3D_SIZE_X 10;
#define LOOKUP_TABLE_3D_SIZE_Y 10;

typedef struct {
    float keys_x[LOOKUP_TABLE_3D_SIZE_X];
    float keys_y[LOOKUP_TABLE_3D_SIZE_Y];
    float values[LOOKUP_TABLE_3D_SIZE_X][LOOKUP_TABLE_3D_SIZE_Y];
} lookup_table_3d_t;

/**
 * Performs any initialization needed to perform lookups. Typically this will
 * entail finding the largest and smallest values in the table.
 */
void init(const lookup_table_3d_t& table);
float get(const lookup_table_3d_t& table, float x, float y);

#endif
