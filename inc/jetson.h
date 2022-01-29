#ifndef __JETSON__H__
#define __JETSON__H__


#define BMCU_PWR_BTN    7   // in
#define SHUTDOWN_REQ    9   // in
#define BMCU_ACOK       11  // in
#define BMCU_POWER_EN   13  // out
#define RESET_N         16  // in

#define TIMER_DELAY     50
#define PWR_OFF_DELAY   (10000/TIMER_DELAY)
#define PWR_ON_DELAY    1


#endif // __JETSON__H__
