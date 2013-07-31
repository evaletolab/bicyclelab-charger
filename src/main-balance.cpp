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


#ifdef CONFIG_WITH_EEPROM
//#include <EEPROM.h>
#endif



#ifdef CONFIG_WITH_ECLIPSE
extern "C" void __cxa_pure_virtual() {  }
#endif




void setup()   {                
  // initialize the digital pin as VCC
  pinMode(P_SIN, INPUT);     
  digitalWrite(P_SIN, LOW);
  //pinMode(P_SOUT, OUTPUT);     
  //pinMode(P_GND, OUTPUT);     
  //digitalWrite(P_VCC, HIGH);
  //digitalWrite(P_GND, LOW);

  //
  //pullup the AD   
  //pinMode(A_BATTERY, INPUT);
  //digitalWrite(A_BATTERY, HIGH);
  
 



#ifdef CONFIG_WITH_PRINT
  Serial.begin(38400);
	Serial.println("Booting ");
	Serial.print("CPU: ");
	Serial.println(OSCCAL);
#endif

}
 

int o=LOW;

void loop(){		   
	//
	// check if battery is ok
	float vcc=avr_internal_vcc();
	float vtemp=avr_internal_temp();
  int loop=255;
  int pwm=0, dim=0;
#ifdef CONFIG_WITH_PRINT
  energy_debug_on();
	Serial.print(" - Temp. sens: ");
	Serial.println(vtemp,2);
	Serial.print(" - Volt. sens: ");
	Serial.println(vcc,2);
  Serial.println("DEBUG MODE! -- add power +24.3mA");
  energy_debug_off();
#endif  
  while (loop--){
    energy_sleep(100);
  	if (analogRead(2)==LOW)
      digitalWrite(P_SOUT, HIGH);
    else
      digitalWrite(P_SOUT, LOW);
  }
	
}




