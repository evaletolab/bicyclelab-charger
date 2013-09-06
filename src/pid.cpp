#include "config.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "pid.h"


 
/*
 * This function initializes the data in a PID structure.
 * Min and max define limit of the setpoint value
 */
void pid_init(PID& pid, float min, float max, float kp, float ki, float kd){
  pid.e=pid.i=0.0;
  pid.min = min;
  pid.max = max;
  pid.kp  = kp;
  pid.ki  = ki;
  pid.kd  = kd;
}


void pid_reset(PID& pid){
  pid.e=pid.i=0.0;
}
 
/*
 * This function updates the value of the manipulated variable (PWM)
 * based on the current state of the PID loop.
 * sp: the setpoint, pv, the process value, PID
 */
float pid_update(PID& pid, float sp, float pv){
  float temp;
  float e;
  float p;
  float manp;
  float tmpi;
  e = pid.e;
  pid.e = sp - pv;
  tmpi = pid.i + pid.e;
  
  //
  //bound the integral
  manp = pid.kp * pid.e + pid.ki * tmpi + pid.kd * (e - pid.e);
  if ( (manp < pid.max) && (manp > pid.min) ){
    pid.i = tmpi;
  } else if ( manp > pid.max ){
    manp = pid.max;
  } else if ( manp < pid.min ){
    manp = pid.min;
  }
  return manp;
}
 
