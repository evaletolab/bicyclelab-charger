#include <TimerOne.h>
#include <PID_v1.h>

/**
 * read keyboard
 */


int readKb(int init) { 
  int pwm=init;

  int v=0;
  byte kb=-1;
  byte sa=Serial.available();
  // if we get a valid byte, read analog ins:
  if (sa == 4) {
     Serial.read();
     Serial.read();
     kb = Serial.read();Serial.read();
  }   
  else if (sa == 3) {
     Serial.read();
     Serial.read();
     kb = Serial.read();  
  }   
  else if (sa > 0) {
     kb = Serial.read();
  }
  
  
  switch(kb){
     // pgup == 53, pgdn == 54  
    case  53:
      pwm+=32;
      if (pwm>255)pwm=255;
      break;
    case  54:
      pwm-=32;
      if (pwm<0)pwm=0;
      break;
    //
    // up == 65
    case  65:
      pwm+=3;
      break;
    //
    // down == 66
    case  66:
      pwm-=3;
      break;

    //
    // right == 67
    case  67:
      break;

    //
    // right == 67
    case  68:
      break;

    //a == 97 A==65

    case  110: // n
      break;    
    case  32: // SPACE
      pwm=(!pwm)?255:0;
      break;
    case  97: // b
      break;
    case  111: // o
      break;
    case  112: // p
      break;
  }
  //Serial.print("KB: ");Serial.println(kb);
  //delay(1000);
  //
  return pwm;
} 

/**
 * Divides a given PWM pin frequency by a divisor.
 *
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 *
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 *
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 *
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

//Define the aggressive and conservative Tuning Parameters
// http://fr.wikipedia.org/wiki/M%C3%A9thode_de_Ziegler-Nichols
// Ku = 2.1, Tu = 50Hz, 0.02s, 
// Kp =  Ku/2.2 =.96  , Ki = (1.2 * Kp) / Pu = .023, Kd=0
//double aKp=0.96, aKi=0.01, aKd=0.0001;
double aKp=1.8, aKi=0.03, aKd=0.0064;

// Kp = 0.6 *Ku = 1.26, Ki = (2 * Kp) / Pu = 0.05, Kd = (Kp * Pu) / 8 = 7.875
//double aKp=1.26, aKi=0.05, aKd=0.001;

// Kp = 0.2 *Ku = .41 , Ki = (2 * Kp) / Pu = 0.0168, Kd = (Kp * Pu) / 3 = 8.5
//double cKp=.41, cKi=0.01, cKd=.001;


// Set pin 6's PWM frequency to 62500 Hz (62500/1 = 62500)
// Note that the base frequency for pins 5 and 6 is 62500 Hz  
// *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
// *     millis() functions to stop working. Other timing-related
#define         P_PWM 6

//
// V_REF as master, I_MAX is normalized 
#define         V_REF 1.0
#define         I_MAX 0.8
#define         O_V 1.2
#define         O_I 1.0
#define         PRINT_TICK 200
#define         PRINT 1


//Specify common PID values
double fb,pwm ;
//Specify PID values for Voltage
double ref_v=V_REF*1023.0/5.0, fb_v,  vfb;

//Specify values for current. Current is normalized with V_REF
double  fb_i,  ifb, norm_i=V_REF/I_MAX;

PID pid_vfb(&fb, &pwm, &ref_v, aKp, aKi, aKd, DIRECT);

//
// print, 
int cpt=0, agg=1;

void setup() {
  //
  // time to flash
  delay(2000);

  setPwmFrequency(P_PWM, 1);
  analogWrite(P_PWM, 0); 
  
  pid_vfb.SetMode(AUTOMATIC);
  
  //
  //recompute PID every 5ms
  pid_vfb.SetSampleTime(7);
  //
  // avoid the MOSFET always on
  pid_vfb.SetOutputLimits(0,253);
#if PRINT  
  Serial.begin(115200);
#endif
  
}

float vmin=200.0,vmax=0.0;
float p_fb=0, p_pwm=0, p_vfb=0, p_ifb=0;


void loop() {
  // put your main code here, to run repeatedly: 
  while(true){
    
    //
    // read voltage feedback
    fb_v=analogRead(0);
    vfb=(5.0/1023.0)*fb_v;

    //
    // security trigger on over voltage
    if (vfb>O_V){
      pwm=0.0;
#if PRINT      
      Serial.print("OVER V: ");Serial.println(vfb,2);
#endif      
      analogWrite(P_PWM, 0 );
      //delay(1000);
      continue;      
    }

    //
    // read current feedback and normalize it 
    fb_i=analogRead(1);
    ifb=(5.0/1023.0)*fb_i;
    
    //
    // security trigger on over current
    if (ifb>O_I){
#if PRINT      
      Serial.print("OVER I: ");Serial.println(ifb,2);
#endif      
      pwm=0.0;
      analogWrite(P_PWM, 0 );
      //delay(1000);
      continue;      
    }
    
    //
    // limit on voltage or current
    fb=max((fb_i*norm_i),fb_v);
    //fb=fb_v;
    
    //
    // smooth measures for display
    p_ifb=p_ifb*.8+ifb*.2;
    p_vfb=p_vfb*.8+vfb*.2;
    p_fb =p_fb*.8+fb*.2;
    p_pwm=p_pwm*.8+pwm*.2;
    



    if(++cpt>PRINT_TICK){
#if PRINT      
      Serial.print("PWM: ");Serial.print(p_pwm,0);
      Serial.print(", A: ");Serial.print(agg);
      Serial.print(", VREF: ");Serial.print(V_REF,1);
      Serial.print(", VOUT: ");Serial.print(p_vfb,2);
      Serial.print(", IFB: ");Serial.print(p_ifb,2);
      Serial.print(", FB: ");Serial.print(p_fb*(5.0/1023.0),1);
      Serial.print(", RI: ");Serial.println((vmin)*(5.0/1023.0),2);
#endif
      vmin=V_REF;
      vmax=0.0;
      cpt=0;
    }
    vmin=min(vmin,p_fb);
    vmax=max(vmax,p_fb);
#if 0
    if (abs(V_REF-p_fb*(5.0/1023.0))>.16){
      if (!agg){
        pid_vfb.SetTunings(aKp, aKi, aKd);
        agg=true;
      }
    }else if(agg){
      pid_vfb.SetTunings(cKp, cKi, cKd);
      agg=false;
    }
#endif    
    pid_vfb.Compute();
    analogWrite(P_PWM, (int)(pwm) ); 


    //delay(2L);
  }

}
