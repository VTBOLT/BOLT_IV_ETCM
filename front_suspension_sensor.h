#ifndef FRONT_SUSPENSION_SENSOR_H_
#define FRONT_SUSPENSION_SENSOR_H_



#include "driverlib.h"
#include "device.h"


//Function Prototypes.
void initFrontSuspensionSensor(void);
uint16_t getSensorPosition(void);
bool getWheelie(void);

#endif
