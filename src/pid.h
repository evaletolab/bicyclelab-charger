/*
 *
 */


#ifndef PID_H_
#define PID_H_


#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  float max   /*! Max manipulated value */;
  float min   /*! Miniumum manipulated value */;
  float e     /*! Error value */;
  float i     /*! Integrator value */;
  float kp    /*! Proportional constant */;
  float ki    /*! Integrator constant */;
  float kd    /*! Differential constant */;
} PID;


//
//
// This function initializes the data in a PID structure.
void    pid_init        (PID &pid, float min, float max, float kp, float ki, float kd);


// This function updates the value of the manipulated variable (PWM)
// based on the current state of the PID loop.
// sp; setpoint (expected value), pv; the process value, PID
float   pid_update      (PID& pid, float sp, float pv);


void    pid_reset       (PID& pid);

#ifdef __cplusplus
}
#endif
#endif

