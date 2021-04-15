#ifndef SUSPENSION_SENSOR_H_
#define SUSPENSION_SENSOR_H_



#include "driverlib.h"
#include "device.h"

#define FRONT_SUSP_SENSOR_ADC_BASE ADCB_BASE
#define FRONT_SUSP_SENSOR_ADC_SOC ADC_SOC_NUMBER0
#define FRONT_SUSP_SENSOR_ADC_CHANNEL ADC_CH_ADCIN5
#define FRONT_SUSP_SENSOR_ADC_INT INT_ADCB1
#define FRONT_SUSP_SENSOR_ADC_INT_NUM ADC_INT_NUMBER1
#define FRONT_SUSP_SENSOR_EPWM_BASE EPWM1_BASE
#define FRONT_SUSP_SENSOR_EPWM_SOC EPWM_SOC_A

#define REAR_SUSP_SENSOR_ADC_BASE ADCB_BASE
#define REAR_SUSP_SENSOR_ADC_SOC ADC_SOC_NUMBER1
#define REAR_SUSP_SENSOR_ADC_CHANNEL ADC_CH_ADCIN4
#define REAR_SUSP_SENSOR_ADC_INT INT_ADCB2
#define REAR_SUSP_SENSOR_ADC_INT_NUM ADC_INT_NUMBER2
#define REAR_SUSP_SENSOR_EPWM_BASE EPWM2_BASE
#define REAR_SUSP_SENSOR_EPWM_SOC EPWM_SOC_A

//Function Prototypes.
void initFrontSuspensionSensorADC(void);
void initFrontSuspensionSensorADCSOC(void);
void initFrontSuspensionSensorEPWM(void);
void initFrontSuspensionSensor(void);
uint16_t getFrontSensorPosition(void);
bool getFrontWheelieStatus(void);

void initRearSuspensionSensorADC(void);
void initRearSuspensionSensorADCSOC(void);
void initRearSuspensionSensorEPWM(void);
void initRearSuspensionSensor(void);
uint16_t getRearSensorPosition(void);
bool getRearWheelieStatus(void);

#endif
