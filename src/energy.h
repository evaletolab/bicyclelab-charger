/* check this:
 * http://www.sparkfun.com/tutorials/309
 */


#ifndef ENERGY_H_
#define ENERGY_H_


#ifdef __cplusplus
extern "C"{
#endif

//#  ENERGY
//#  ATtiny45 power requirements:
//# 
//#  Power-Down at Vcc=5.0V, Sleeping-BOD disabled:
//#    BOD Off:   0.9μA
//#    BOD 4V3:  19.3μA
//#    BOD 2V7:  21.2μA
//#    BOD 1V8:  23.9μA 
//#  -----
//#  Internal bandgap reference current consumption 15uA 
//#   
//#  DebugSerial consumption 24ma @5V
//# P.182 Attiny84, Supply current of I/O modules
//# PRR bit  2V - 1MHz | 3V - 4MHz |   5V - 8MHz
//# -------------------+-----------+------------
//# PRTIM1   1.6 μA    | 11 μA     |    48μA
//# PRTIM0   4.4 μA    | 29 μA     |   120μA
//# PRUSI    1.6 μA    | 11 μA     |    48μA
//# PRADC    8.0 μA    | 55 μA     |   240μA

// http://code.google.com/p/tinkerit/wiki/SecretThermometer

//
//every attiny45 (and others) has an internal temperature sensor	
float 		avr_internal_temp		();

//
//read vcc
double 		avr_internal_vcc		();

long 			energy_millis				();
void 			energy_deep_sleep		();
void 			energy_sleep				(int ms);
void 			energy_delay				(int ms);
void			energy_save_off			();
void			energy_save_on			();
void 			energy_init					();
void 			energy_switch_off		();
void 			energy_switch_on		();
void 			energy_debug_on		  ();
void 			energy_debug_off	  ();

//
// check is the system boot in a dirty state. return 0,1
uint8_t		energy_dirty_start	();

//
// prescaler
void 			energy_set_prescaler				(short mode);
short			energy_get_prescaler				();
short 		energy_get_prescaler_factor	();

#ifdef __cplusplus
}
#endif
#endif

