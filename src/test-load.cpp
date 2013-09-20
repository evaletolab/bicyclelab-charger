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
#include "charger.h"
#include "energy.h"
#ifdef TINY
#include <core_timers.h>
#endif

#ifdef CONFIG_WITH_EEPROM
//#include <EEPROM.h>
#endif



#ifdef CONFIG_WITH_ECLIPSE
extern "C" void __cxa_pure_virtual() {  }
#endif



static CHARGER charger;

void setup()   {                

  //
  //pullup the AD   
  pinMode(A_VFB, INPUT);
  digitalWrite(A_VFB, HIGH);

  pinMode(A_IFB, INPUT);
  digitalWrite(A_IFB, HIGH);

  //pulldown pwm
  pinMode(P_PWM, OUTPUT);
  //digitalWrite(P_PWM, LOW);
  analogWrite(P_PWM,0);  
  
  
  pinMode(P_VCC5, INPUT);
  digitalWrite(P_VCC5, HIGH);
  
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
  // http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM
  // Set pin 6's PWM frequency to 62500 Hz (62500/1 = 62500)
  // Note that the base frequency for pins 5 and 6 is 62500 Hz  
  // *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
  // *     millis() functions to stop working. Other timing-related
  // *   - Pins 5 and 6 are paired on timer0
  // *   - Pins 9 and 10 are paired on timer1
  // *   - Pins 3 and 11 are paired on timer2
  TCCR0B = TCCR0B & 0b11111000 | 0x01;
#endif
   
  

  charger_init(charger);

#ifdef CONFIG_WITH_PRINT
  Serial.begin(115200);
	Serial.println("Booting ");
#endif

}


void loop(){		   
  
  

//Specify values for current. Current is normalized with V_BATT
double  fb_i,  ifb, vfb, fb_v;
  
  //
  //
  // checking for plugged input == VIN
  // calibrate the openvoltage value
#ifdef CONFIG_WITH_PRINT      
  Serial.println("START CALIBRATE: ");
#endif      

#ifdef CONFIG_WITH_PRINT      
  //pinMode(2, OUTPUT);
  //digitalWrite(2, LOW);
  //short odd=LOW;

  digitalWrite(P_SW, HIGH);
  // don't load directly, preload with a low pwm
  analogWrite(P_PWM, (int)10 );
  delay(TIMER_WAIT*10);   
  analogWrite(P_PWM, (int)20 ); 

  //
  // one loop is about (on arduino 16Mhz)
  // - 920us, 1087Hz (1x avg_read without Serial.print)
  // - 1.8ms, 555Hz (2x avg_read without Serial.print=
  // - 3.3ms, 306Hz (only with Serial.print)
  
  while(true){
// Helper to measure loop time
//    digitalWrite(2,odd?HIGH:LOW);
//    odd=!odd;
  
    ifb=avg_read(A_IFB,fb_i);
    vfb=avg_read(A_VFB,fb_v);
//    Serial.print(open_pwm,2);
    Serial.print(" : ");
    Serial.print(vfb*V_FACTOR,2);
    Serial.print(" : ");
    Serial.println(ifb*I_FACTOR,3);
  }
#endif      
  
}




