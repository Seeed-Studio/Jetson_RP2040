#ifndef __JETSON__H__
#define __JETSON__H__


#define BMCU_PWR_BTN    7   // in
#define SHUTDOWN_REQ    9   // in
#define BMCU_ACOK       11  // in
#define BMCU_POWER_EN   13  // out
#define RESET_N         16  // in
#define ADC_5V          26
#define ADC_3V          27

#define TIMER_PWR_BTN   50
#define PWR_OFF_DELAY   (10000/TIMER_PWR_BTN)
#define PWR_ON_DELAY    1

#define TIMER_PWR_DETECT    5
#define PWR_5V_LOWER        1000
#define PWR_3V_LOWER        1000

void jetson_init(void);
void jetson_auto_on(void);
void jetson_pwr_btn(void);
void jetson_pwr_detect(void);


#endif // __JETSON__H__
