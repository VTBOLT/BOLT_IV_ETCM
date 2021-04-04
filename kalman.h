// kalman.h

#ifndef kalman_H_
#define kalman_H_

 float kalmanGain( float eEST,  float eMEA);
 float kalmanEstimate( float prevEST,  float eEST,  float eMEA,  float measurement);
 float newESTerror( float prevESTe,  float eMEA);


#endif
