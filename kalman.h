// kalman.h

#ifndef kalman_H_
#define kalman_H_

volatile float kalmanGain(volatile float eEST, volatile float eMEA);
volatile float kalmanEstimate(volatile float prevEST, volatile float eEST, volatile float eMEA, volatile float measurement);
volatile float newESTerror(volatile float prevESTe, volatile float eMEA);


#endif
