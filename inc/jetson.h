#ifndef __JETSON__H__
#define __JETSON__H__


#define JETSON_VER      0x10


#define BMCU_PWR_BTN    7   // in
#define SHUTDOWN_REQ    9   // in
#define BMCU_ACOK       11  // in
#define BMCU_POWER_EN   13  // out
#define RESET_N         16  // in
#define ADC_5V          26
#define ADC_3V          27

#define PWR_STABLE_WAIT 1000 // 1000ms

#define TIMER_PWR_BTN   50  // 50ms
#define PWR_OFF_DELAY   (10000/TIMER_PWR_BTN) // 10s
#define PWR_ON_DELAY    1   // 50ms

#define TIMER_PWR_DETECT    1       // 1ms
#define PWR_5V_LOWER        1343    // 4.2v
#define PWR_3V_LOWER        1400    // 2.8v

void jetson_init(void);
void jetson_auto_on(void);
void jetson_pwr_btn(void);


#endif // __JETSON__H__
