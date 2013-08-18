/**
 * read value from cell, balance if needed, inform the status 
 *  roadmap:
 *  - read value from cell
 *  - get status from stacked cell
 *  - set status from next cell
 * 
 *  
 */

#ifdef ARDUINO
# include <Arduino.h>
#else
# include <WProgram.h>
#endif
#include "config.h"
#include "energy.h"
#ifdef TINY
#include <core_timers.h>
#endif
#include <PID_v1.h>

#ifdef CONFIG_WITH_EEPROM
//#include <EEPROM.h>
#endif



#ifdef CONFIG_WITH_ECLIPSE
extern "C" void __cxa_pure_virtual() {  }
#endif


//Define the aggressive and conservative Tuning Parameters
// http://fr.wikipedia.org/wiki/M%C3%A9thode_de_Ziegler-Nichols
// Ku = 2.1, Tu = 50Hz, 0.02s, 
// Kp =  Ku/2.2 =.96  , Ki = (1.2 * Kp) / Pu = .023, Kd=0
//double aKp=0.96, aKi=0.01, aKd=0.0001;
double aKp=1.8, aKi=0.03, aKd=0.0064;


//Specify common PID values
double fb,pwm ;
//Specify PID values for Voltage
double ref_v=V_REF*1023.0/VCC_REF, fb_v,  vfb;

//Specify values for current. Current is normalized with V_REF
double  fb_i,  ifb, norm_i=I_NORM;

PID pid_vfb(&fb, &pwm, &ref_v, aKp, aKi, aKd, DIRECT);

void setup()   {                

  //
  //pullup the AD   
  pinMode(A_VFB, INPUT);
  digitalWrite(A_VFB, HIGH);

  pinMode(A_IFB, INPUT);
  digitalWrite(A_IFB, HIGH);

  //pulldown pwm
  pinMode(P_PWM, OUTPUT);
  digitalWrite(P_PWM, LOW);
  
  //
  // charger is on
  pinMode(P_SW, OUTPUT);
  digitalWrite(P_SW, LOW);

#ifdef TINY
  //
  // setup timer to the maximum speed at 52Khz  
  OSCCAL = 0xFF;  
  Timer1_SetWaveformGenerationMode(Timer1_Fast_PWM_FF);
  Timer1_ClockSelect(Timer1_Prescale_Value_1);
#else 
  // Set pin 6's PWM frequency to 62500 Hz (62500/1 = 62500)
  // Note that the base frequency for pins 5 and 6 is 62500 Hz  
  // *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
  // *     millis() functions to stop working. Other timing-related
  TCCR0B = TCCR0B & 0b11111000 | 0x01;
#endif
   
  analogWrite(P_PWM, 0);  
  
  pid_vfb.SetMode(AUTOMATIC);
  
  //
  //recompute PID every 6ms
  pid_vfb.SetSampleTime(6);
  //
  // set maximum duty cycle (55%)
  pid_vfb.SetOutputLimits(1,140);


#ifdef CONFIG_WITH_PRINT
  Serial.begin(115200);
	Serial.println("Booting ");
#endif

}
 


void loop(){		   
	//
	// check if battery is ok
#ifdef CONFIG_WITH_PRINT
	float vcc=avr_internal_vcc();
	float vtemp=avr_internal_temp();
  energy_debug_on();
	Serial.print(" - Temp. sens: ");
	Serial.println(vtemp,2);
	Serial.print(" - Volt. sens: ");
	Serial.println(vcc,2);
  Serial.println("DEBUG MODE! -- add power +24.3mA");
  energy_debug_off();
#endif  
  float p_fb=0, p_pwm=0, p_vfb=0, p_ifb=0;
  int odd=false, checking=true, charging=true;
  
  //
  // checking battery before starting the charge 
  analogWrite(P_PWM, 1);  
  delay(TIMER_TICK);
  
  while(checking){
    //
    // switch on charger
    digitalWrite(P_SW, HIGH);
    delay(TIMER_TICK);
    
    //
    // checking shortcut or unpluged
    fb_i=analogRead(A_IFB);
    ifb=(VCC_REF/1023.0)*fb_i;
    if (ifb>O_I){
      //on current to high, switchoff charger
#ifdef CONFIG_WITH_PRINT      
      Serial.print("OVER I: ");Serial.println(ifb,2);
#endif      

      digitalWrite(P_SW,LOW);  
      analogWrite(P_PWM, 0);  
      delay(TIMER_WAIT);
      continue;
    }

    //
    // checking unpluged
    analogWrite(P_PWM, 3);  
    delay(TIMER_TICK);
    fb_v=analogRead(A_VFB);
    vfb=(VCC_REF/1023.0)*fb_v;
    if (vfb>O_V){
      // on volt to high switchoff charger
#ifdef CONFIG_WITH_PRINT      
      Serial.print("OVER V: ");Serial.println(vfb,2);
#endif      
      digitalWrite(P_SW,LOW);  
      analogWrite(P_PWM, 0);  
      delay(TIMER_WAIT);
      continue;
    }
    if (vfb<=1.0){
#ifdef CONFIG_WITH_PRINT      
      Serial.print("STANDBY ");Serial.println(vfb,2);
#endif      
      analogWrite(P_PWM, 1);  
      delay(TIMER_WAIT);
      continue;
    }
    checking=false;
  }     
  
#ifdef CONFIG_WITH_PRINT      
    Serial.println("START CHARGING: ");
#endif      


  //
  // main loop, charging
  while(charging){
  
    // starting charging
    digitalWrite(0,odd);odd=!odd;
    digitalWrite(P_SW, HIGH);
    
    //
    // read voltage feedback
    fb_v=analogRead(A_VFB);
    vfb=(VCC_REF/1023.0)*fb_v;

    //
    // security trigger on over voltage
    if (vfb>O_V){
#ifdef CONFIG_WITH_PRINT      
      Serial.print("OVER V: ");Serial.print(vfb,2);
      Serial.print(" I: ");Serial.println(ifb,2);
#endif      
      pwm=1;
      analogWrite(P_PWM, (int)(pwm) ); 
      digitalWrite(P_SW, LOW);
      delay(TIMER_WAIT);
      continue;      
    }

    //
    // read current feedback and normalize it 
    fb_i=analogRead(A_IFB);
    ifb=(VCC_REF/1023.0)*fb_i;
    
    //
    // security trigger on over current
    if (ifb>O_I){
#ifdef CONFIG_WITH_PRINT      
      Serial.print("OVER I: ");Serial.print(ifb,2);
      Serial.print(" V: ");Serial.println(vfb,2);
#endif      
      pwm=1;
      analogWrite(P_PWM, (int)(pwm) ); 
      digitalWrite(P_SW, LOW);
      delay(TIMER_WAIT);
      continue;      
    }
    
    //
    // check if cherging is done
    //if (vfb>=42.0 && ifb<=){
    //}
    
    //
    // limit on voltage or current (or power UxI)
    fb=max((fb_i*norm_i),fb_v);
    //fb=fb_v;
    
    
    pid_vfb.Compute();
    analogWrite(P_PWM, (int)(pwm) ); 


  
  }
}




