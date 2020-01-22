
#ifndef IMU_H_
#define IMU_H_

#define GPIO_CFG_SCITX  GPIO_18_SCITXDB
#define GPIO_CFG_SCIRX  GPIO_18_SCIRXDB
#define GPIO_SCITX      18U
#define GPIO_SCIRX      19U

#define SCIB_BASE       SCIB_BASE
#define SCI_BAUD        115200U


//Function Prototypes.

__interrupt void readIMUDataISR(void);
void initSCIBFIFO(void);
void pulseSyncIn(void);
void enableGPIO(void);
void initIMUInterrupt(void);


float yaw;
float pitch;

#endif 
