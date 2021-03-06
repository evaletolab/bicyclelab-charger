/*
 *
 */


#ifndef CHARGER_H_
#define CHARGER_H_

// precalculed values 
// ------------------
//
// --> 48.0/11.0=4.36, 
// --> 42.0/11.0=3.818, 
// --> 43.0/11.0=3.909, 
// -->  3A/0.84=3.57
// --> .3A/0.84=0.357
// --> 80W/(0.84*11.0)=8.66
// --> 75W/(0.84*11.0)=7.74
// --> 70W/(0.84*11.0)=7.58
// --> 65W/(0.84*11.0)=7.03
// --> 60W/(0.84*11.0)=6.49
// --> 50W/(0.84*11.0)=5.41
// --> 45W/(0.84*11.0)=4.87
// --> 40W/(0.84*11.0)=4.33
// --> 35W/(0.84*11.0)=3.78
// --> 30W/(0.84*11.0)=3.25
// --> 25W/(0.84*11.0)=2.71
// --> 20W/(0.84*11.0)=2.16
// --> 15W/(0.84*11.0)=1.62


//
// select charging mode and specification
//#define MODE_462V  
#define MODE_420V  
//#define MODE_200V  

#define         V_FACTOR 11.00
#define         I_FACTOR  0.84
#ifdef MODE_462V
# define         V_BATT 4.2
# define         O_V 4.3
# define         U_V 1.0
# define         V_IN 1.72

// P_MAX 45W
# define         P_MAX 4.87
// I 3A
# define         O_I 3.57
// I 0.03A
# define         I_BATT_CHARGED 0.0357

# define         MAX_PWM 120
# define         MIN_PWM 0
# define         OPEN_PWM 13
# define         OPEN_MAX_PWM 27
#endif

#ifdef MODE_420V
# define         V_BATT 3.909
# define         O_V 4.3636
# define         U_V 1.0
# define         V_IN 1.72

// P_MAX 75W /70W /65W / 45W / 40W
//# define         P_MAX 7.74
# define         P_MAX 7.58
//# define         P_MAX 7.03
//# define         P_MAX 4.87
//# define         P_MAX 4.33
// I 3A
# define         O_I 3.57
// I 0.03A
# define         I_BATT_CHARGED 0.0457

# define         MAX_PWM 153
# define         MIN_PWM 0
# define         OPEN_PWM 13
# define         OPEN_MAX_PWM 27
#endif


#ifdef MODE_200V
# define         V_BATT 1.818
# define         O_V 1.909
# define         U_V 1.0
# define         V_IN 0.9

// P_MAX 20W
# define         P_MAX 2.16
// I 1.5A
# define         O_I 1.79
// I 0.03A
# define         I_BATT_CHARGED 0.0357

# define         MAX_PWM 120
# define         MIN_PWM 0
# define         OPEN_PWM 8
# define         OPEN_MAX_PWM 27
#endif


#define         TEMP_MAX 60.0  
#define         I_NORM (V_BATT/O_I)
#define         P_NORM (V_BATT/P_MAX)
#define         VCC_REF 5.0

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
	double vfb;   //voltage feedback
	double avg_vfb;
	double dfb_v; // digital value
	
	double ifb;   //current feedback
	double avg_ifb;
	double dfb_i; // digital value

	double fb;    //pid setpoint  (feedback)
	double pwm;   //pid process value (boost pwm)
	double open_pwm; //open voltage boost pwm
	
	double temp;
	double sp;    // the pid expected value 
	
} CHARGER;

#define A2D(x)  (x*1023.0/VCC_REF)
#define D2A(x)  (x*VCC_REF/1023.0)

//
//
// read analog pin and return avg for real and digital values
float   avg_read              (short p, double& digital);

//
//
// init charger
int     charger_init          (CHARGER& charger);


//
//
// reset charger, 
// set pwm (MIN_PWM or OPEN_PWM) , 
// pid must be cleared
void    charger_reset         (CHARGER& charger, int pwm);


//
//
// verify mosfet
// if PWM >OPEN_PWM && vout<V_IN && vout >0 => BUG => reset(0)
// TODO the best way todo that is to measure a 0.01Ohm on the mosfet drain
void    charger_check_mosfet  (CHARGER& charger);

//
//
// checking when output is switched off
// detect internal issue like undervoltage, over current, over temperature 
// or unplugged input (for debug only)
int 		charger_openvoltage		(CHARGER &charger);




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
int 		charger_checking		(CHARGER &charger);


//
// main loop, charging
// ===================
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
int     charger_mainloop    (CHARGER& charger);

#ifdef __cplusplus
}
#endif
#endif

