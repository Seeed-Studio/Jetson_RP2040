#ifndef __JETSON__H__
#define __JETSON__H__


#define JETSON_VER      0x11

#define SYS_5V_EN       6   // out
#define BMCU_PWR_BTN    7   // in
#define SHUTDOWN_REQ    9   // in
#define BMCU_ACOK       11  // in
#define BMCU_POWER_EN   13  // out
#define TEST_POINT      14  // out
#define SATA_PwrEN2     15  // out
#define RESET_N         16  // in
#define BEEPER          17  // out

#define ADC_5V_GPIO     26  // ADC0
#define ADC_5V_CH       0
#define ADC_3V_GPIO     27	// ADC1 - not used
#define ADC_3V_CH       1
#define ADC_12V_GPIO    28	// ADC2
#define ADC_12V_CH      2


#define PWR_STABLE_WAIT 500 // 500ms

#define TIMER_PWR_BTN   50  // 50ms
#define PWR_OFF_DELAY   (10000/TIMER_PWR_BTN) // 10s
#define PWR_ON_DELAY    1   // 50ms

#define PWR_5V_LOW      1700   // 4.3v
#define PWR_3V_LOW      1400   // 2.8v

#define DEBUG
#ifdef DEBUG
#define DBG_PRINT(format, x...)		printf("%s:"format, __func__, ##x)
#else
#define DBG_PRINT(format, x...)
#endif


void jetson_init(void);
void jetson_wait_5v(void);
void jetson_auto_on(void);
void jetson_pwr_btn(void);


#endif // __JETSON__H__
