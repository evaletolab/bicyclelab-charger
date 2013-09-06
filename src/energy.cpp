#ifdef ARDUINO
# include <Arduino.h>
#else
# include <WProgram.h>
#endif
#include "config.h"
#include "wiring_private.h"
#include "pins_arduino.h"

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "energy.h"




#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

static float 		_VREF=0.0;
static short 		_PRES=0;
static uint8_t  _STATE=0;

#define PRESCALER_FACTOR(P) ((uint16_t)(1 << P))

ISR(ADC_vect)
{
} 


SIGNAL(WDT_vect) {
  wdt_disable();
  wdt_reset();
#if WDTCR
  WDTCR &= ~_BV(WDIE);
#else  
  WDTCSR &= ~_BV(WDIE);
#endif  
}

inline long 	energy_millis(){
	return (long)(millis()*PRESCALER_FACTOR(_PRES));
}


// prescale of 2² (4)  5,3mA with a328p (prescale 0 => 8,5mA)
void energy_set_prescaler(short mode){
  cli();
  CLKPR=bit(CLKPCE);
  CLKPR=(byte)mode;
  _PRES=mode;
  sei();
}

short energy_get_prescaler(){
	return _PRES;
}
short energy_get_prescaler_factor(){
	return PRESCALER_FACTOR(_PRES); // == pow(2,_PRES)
}

uint8_t	energy_dirty_start(){
	
	uint8_t state=0;
	//EEPROM_read(0, &state, 1);
	return state;
}


void energy_switch_off(){
	_STATE=0;
	//
	//the system is going down, save the state
	//EEPROM_write(0, &_STATE, 1);


	//
	//reset the cpu to initial value
  energy_set_prescaler(0);
  
  
}

void energy_switch_on(){
	_STATE=1;
	//
	//the system is up, save the state
	//EEPROM_write(0, &_STATE, 1);
	
	//
	//(lowdown the cpu)? to save energy
  energy_set_prescaler(SET_PRESCALER);
	power_timer0_enable();

}

void energy_deep_sleep(){
		delay(5);
	  energy_set_prescaler(0);
	  energy_save_on();
		power_timer0_disable();
		while(true)energy_sleep(4000);
}

void energy_save_off(){
	power_timer0_enable();
	power_timer1_enable();
//	power_twi_enable();
#if PRSPI  
	power_spi_enable();
#endif	
  power_adc_enable();
#if PRUSI  
  power_usi_enable();
#endif  
}

void energy_save_on(){
  power_adc_disable();
#if PRUSI  
  power_usi_disable();
#endif  
#if PRSPI  
	power_spi_disable();
#endif	
//	power_twi_disable();
	power_timer0_disable();
	power_timer1_disable();
}

void __enable(){
	power_timer0_enable();
	power_timer1_enable();
//	power_twi_enable();
//	power_spi_enable();
  power_adc_enable();
#if PRUSI  
  power_usi_enable();
#endif  
}

void __disable(){
#if PRADC  
  power_adc_disable();
#endif
#if PRUSI  
  power_usi_disable();
#endif
#if PRSPI  
	power_spi_disable();
#endif
#if PRTWI
	power_twi_disable();
#endif
#if PRVRM
  power_vrm_enable();
#endif
#if PRTIM0
	power_timer0_disable();
#endif
#if PRTIM1
	power_timer1_disable();
#endif
#if PRTIM2
	power_timer2_disable();
#endif

}

void energy_delay(int t){
	t/=PRESCALER_FACTOR(_PRES);
  long l=millis()+t;
  while(millis()<l){
	  set_sleep_mode(SLEEP_MODE_IDLE);
  	sleep_mode();
  }  
}


void __sleep_high(uint8_t wdt_period) {
  wdt_enable(wdt_period);
  wdt_reset();
#if WDTCR
  WDTCR |= _BV(WDIE);
#else  
  WDTCSR |= _BV(WDIE);
#endif  

  do {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        cli();
        sleep_enable();
        sleep_bod_disable();
        sei();
        sleep_cpu();
        sleep_disable();
        sei();
  } while (0);
  wdt_disable();
#if WDTCR
  WDTCR &= ~_BV(WDIE);  
#else  
  WDTCSR &= ~_BV(WDIE);  
#endif  
}

void energy_sleep(int milliseconds) {
	__disable();
  while (milliseconds >= 8000) { __sleep_high(WDTO_8S); milliseconds -= 8000; }
  if (milliseconds >= 4000)    { __sleep_high(WDTO_4S); milliseconds -= 4000; }
  if (milliseconds >= 2000)    { __sleep_high(WDTO_2S); milliseconds -= 2000; }
  if (milliseconds >= 1000)    { __sleep_high(WDTO_1S); milliseconds -= 1000; }
  if (milliseconds >= 500)     { __sleep_high(WDTO_500MS); milliseconds -= 500; }
  if (milliseconds >= 250)     { __sleep_high(WDTO_250MS); milliseconds -= 250; }
  if (milliseconds >= 125)     { __sleep_high(WDTO_120MS); milliseconds -= 120; }
  if (milliseconds >= 64)      { __sleep_high(WDTO_60MS); milliseconds -= 60; }
  if (milliseconds >= 32)      { __sleep_high(WDTO_30MS); milliseconds -= 30; }
  if (milliseconds >= 16)      { __sleep_high(WDTO_15MS); milliseconds -= 15; }
  if (milliseconds >= 1)			 { 
    //energy_delay(milliseconds);	
  }
	__enable();
}


void energy_init(){
 
//  ADCSRB = 0;  
}

void energy_debug_on(){
#ifdef P_DEBUG
  pinMode(P_DEBUG, OUTPUT);       
  digitalWrite(P_DEBUG, LOW);
#endif  
}

void energy_debug_off(){
#ifdef P_DEBUG
  pinMode(P_DEBUG, INPUT);       
  digitalWrite(P_DEBUG, HIGH);
#endif  
}


/**
 * -40° = 230LSB, +25° = 300 LSB, +85° = 370 LSB
 * T = k * [(ADCH << 8) | ADCL] + TOS
 */

uint16_t _avr_temp(){
    uint16_t res=0;
#ifdef A_TEMP    
		ADMUX = A_TEMP;			//attinyX4: B100010=> TEMP sensor, 

    delay(1); 
    set_sleep_mode(SLEEP_MODE_ADC);
    //power_adc_disable();
    ADCSRA |=  _BV(ADIF)| _BV( ADIE )| _BV(ADSC); // start A/D conversion, ADC noise reduction, Begin conversion, CLK/128 energy_set_prescaler
    while(ADCSRA & _BV(ADSC)){
            sleep_mode();
    }
    res = ADC;
    ADCSRA &= ~(_BV(ADIE));
    ADCSRA &= ~(_BV(ADIF));
#endif    
    return res;
}


float avr_internal_temp(){
	double t=(double)_avr_temp();
	for(short i=0;i<2;i++)
		t=t*.5+_avr_temp()*.5;
	
//	return (_VREF/1024.0)*(t)/10.00;
	return (t)-TEMPERATURE_CALIBRATE;
}




double avr_internal_vcc() {
#if MUX_VREF
	// read vcc
	// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=80458&start=0&postdays=0&postorder=asc&highlight=adc+bandgap+vcc
  long result;
  //
  // read 1.1V ref against AVcc
  // - ADC_VREF = 0b00000000
  // - MUX_VREF = 0b00100001
  ADMUX = MUX_VREF|ADC_VREF; 
//  ADMUX = _BV(REFS1) | _BV(MUX0) 
  delay(2); // Wait for Vref to settle
  set_sleep_mode(SLEEP_MODE_IDLE);
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC)){
      sleep_mode();
  }
  result = ADC;
  //5V VCC value from internal Vref 1.1 (225.3) => 225.3*1126.4=5V 
  _VREF=(1126.400/result);
  
	return _VREF;
#else
 return 3.3;
#endif 
}



