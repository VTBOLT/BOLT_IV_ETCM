#ifndef IMU_ETCM_H_
#define IMU_ETCM_H_

#include "device.h"
#include "driverlib.h"
#include "leds_etcm.h"

void strobeIMUSyncIn(void);
void initIMUTransfer(void);
void initIMUSyncIn(void);
void initIMUInterrrupt(void);
float getIMUPitch(void);
float getIMURoll(void);
float getIMUYaw(void);
void SCIWriteInt(int intIn);
void SCIWriteChar(uint32_t SCIbase, const char *const dataArray, int dataLength);
void initDebugTransfer(void);


//****************
// Defines
//****************
#define GPIO_CFG_SYNC_IN        GPIO_67_GPIO67
#define GPIO_SYNC_IN            67
#define IMU_FRAME_SIZE          18
#define GPIO_CFG_SCITX_IMU      GPIO_18_SCITXDB
#define GPIO_CFG_SCIRX_IMU      GPIO_19_SCIRXDB
#define GPIO_SCITX_IMU          18U
#define GPIO_SCIRX_IMU          19U


#define SCI_IMU_BASE            SCIB_BASE
#define SCI_IMU_BAUD            115200U

// DEBUG
#define GPIO_CFG_SCITX_DEBUG  GPIO_42_SCITXDA
#define GPIO_CFG_SCIRX_DEBUG  GPIO_43_SCIRXDA
#define GPIO_SCITX_DEBUG      42U
#define GPIO_SCIRX_DEBUG      43U
#define SCI_DEBUG_BASE        SCIA_BASE
#define SCI_DEBUG_BAUD        9600U


//**********
// Globals
//**********



#endif
