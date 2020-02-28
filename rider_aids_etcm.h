/*
 * rider_aids_etcm.h
 *
 *  Created on: Feb 26, 2020
 *      Author: Will Campbell
 */

#ifndef RIDER_AIDS_ETCM_H_
#define RIDER_AIDS_ETCM_H_

bool checkTC(int frontWhlSpd, int backWhlSpd);
bool checkAntiJerk(float roll, int frontWhlSpd, int backWhlSpd);
bool checkAntiWheelie(float pitch, float frontSusp, float backSusp); // maybe make int dep on res



#endif /* RIDER_AIDS_ETCM_H_ */
