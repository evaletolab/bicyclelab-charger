/**
 * read value from cell, balance if needed, inform the status 
 *  roadmap:
 *  - read value from cell
 *  - get status from stacked cell
 *  - set status from next cell
 * 
 *  
 */


#include <WProgram.h>
#include "config.h"
#include "energy.h"
#include <core_timers.h>
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
double  fb_i,  ifb, norm_i=V_REF/I_MAX;

PID pid_vfb(&fb, &pwm, &ref_v, aKp, aKi, aKd, DIRECT);

void setup()   {                

  //
  //pullup the AD   
  pinMode(A_VFB, INPUT);
  digitalWrite(A_VFB, HIGH);

  pinMode(A_IFB, INPUT);
  digitalWrite(A_IFB, HIGH);


  //
  // setup timer to the maximum speed at 52Khz  
  OSCCAL = 0xFF;  
  Timer1_SetWaveformGenerationMode(Timer1_Fast_PWM_FF);
  Timer1_ClockSelect(Timer1_Prescale_Value_1);
   
  analogWrite(P_PWM, 0);  
  
  pid_vfb.SetMode(AUTOMATIC);
  
  //
  //recompute PID every 6ms
  pid_vfb.SetSampleTime(8);
  //
  // set maximum duty cycle (55%)
  pid_vfb.SetOutputLimits(1,140);


#ifdef CONFIG_WITH_PRINT
  Serial.begin(38400);
	Serial.println("Booting ");
	Serial.print("CPU: ");
	Serial.println(OSCCAL);
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
  int odd=false, checking=true;
  
  //
  // checking battery before starting the charge 
  while(checking){
    //
    // switch on charger
    //digitalWrite(P_CHARGE,HIGH);  
    
    //
    // checking shortcut or unpluged
    fb_i=analogRead(A_IFB);
    ifb=(VCC_REF/1023.0)*fb_i;
    if (ifb>O_I){
      //on current to high, switchoff charger

      //digitalWrite(P_CHARGE,LOW);  
      delay(1000);
      continue;
    }

    //
    // checking unpluged
    analogWrite(P_PWM, 10);  
    fb_v=analogRead(A_VFB);
    vfb=(VCC_REF/1023.0)*fb_v;
    if (vfb>O_V){
      // on volt to high switchoff charger
      //digitalWrite(P_CHARGE,LOW);  
      delay(1000);
      continue;
    }
    checking=false;
  }     
  

  //
  // main loop, charging
  while(true){
  
    digitalWrite(0,odd);odd=!odd;
  
    //
    // read voltage feedback
    fb_v=analogRead(A_VFB);
    vfb=(VCC_REF/1023.0)*fb_v;

    //
    // security trigger on over voltage
    if (vfb>O_V && 0){
      pwm=0.0;
#if CONFIG_WITH_PRINT      
      Serial.print("OVER V: ");Serial.println(vfb,2);
#endif      
      analogWrite(P_PWM, 0 );
      //delay(1000);
      continue;      
    }

    //
    // read current feedback and normalize it 
    fb_i=analogRead(A_IFB);
    ifb=(VCC_REF/1023.0)*fb_i;
    
    //
    // security trigger on over current
    if (ifb>O_I && 0){
#if CONFIG_WITH_PRINT      
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
    fb=fb_v;
    
    
    pid_vfb.Compute();
    analogWrite(P_PWM, (int)(pwm) ); 


  
  }
}




