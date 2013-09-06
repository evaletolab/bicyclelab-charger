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
#include "charger.h"
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
  //
  //
  // checking when output is switched off
  // detect internal issue like undervoltage, over current, over temperature 
  // or unplugged input (for debug only)
  if(!charger_openvoltage(charger)) 
    return;

  // Checking battery
  // ================
  // 1)The charger check if a battery is plugged, we assume that battery capacity 
  //   and voltage are hardcoded ( SmartBattery feature is not yet implemented)
  // 2)The charger then checks for over/under battery temperature and over voltage 
  //   and flags an error if any of these are detected, 
  //   -> sending a short error message to the PC.   
  //
  // ----
  // http://batteryuniversity.com/learn/article/charging_lithium_ion_batteries
  // Do not recharge lithium-ion if a cell has stayed at or below 1.5V for more 
  // than a week. Copper shunts may have formed inside the cells that can lead 
  // to a partial or total electrical short. If recharged, the cells might become 
  // unstable, causing excessive heat or showing other anomalies. Li-ion packs 
  // that have been under stress are more sensitive to mechanical abuse, such 
  // as vibration, dropping and exposure to heat.
  if(!charger_checking(charger)) 
    return;
  
  //
  // main loop, charging (never exit function!)
  // ==========================================
  // During all loop (fast & constant charge) the charger check 
  // the temperature and if battery is hot unplugged
  //
  // 2)If OK control proceeds to the Constant Current loop where the PWM signal 
  //   on-time is adjusted up or down until the measured current matches the 
  //   specified fast charge current. 
  //   -> The charge LED flashes quickly during this mode.
  //
  // 3)When the Maximum Charge Voltage setting is reached then the 
  //   mode switches to Constant Voltage and a new loop is entered that uses 
  //   the same method as above to control voltage at the Maximum Charge 
  //   Voltage point. 
  //   -> The LED now flashes more slowly. 
  //
  //   This continues until 30 minutes after the charge current, which is 
  //   decreasing with time in constant voltage mode, reaches the minimum 
  //   (set at 50 mA per 1600 mAH of capacity). 
  //   ->The charger then shuts off and the LED goes on steady.
  // 
  //
  charger_mainloop(charger);
}




