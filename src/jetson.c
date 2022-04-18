/**
 * Copyright (c) 2022 Seeed technology inc.
 *
 * The MIT License (MIT)
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jetson.h"


static struct repeating_timer timer_power_en_fault = { 0 }; // must be static
static unsigned int beeper_cnt = 0, recovery_cnt = 0;
bool power_en_fault_callback(struct repeating_timer *t)
{
	// beeper
	beeper_cnt++;
	if (beeper_cnt < 6) {
		gpio_put(BEEPER, 1);
	}
	else {
		gpio_put(BEEPER, 0);
		if (beeper_cnt >= 10) {
			beeper_cnt = 0;
		}
	}

	// fault recovery
	if (gpio_get(SHUTDOWN_REQ)) {
		recovery_cnt++;
		if (recovery_cnt >= 10) { // 1s
			recovery_cnt = 0;
			cancel_repeating_timer(&timer_power_en_fault);
			gpio_put(BEEPER, 0);
		}
	}
	else {
		recovery_cnt = 0;
	}

	return true;
}

void power_enable(bool enable)
{
	if (enable) {
		if (!gpio_get(SHUTDOWN_REQ)) { // fault
			enable = 0;
			if (timer_power_en_fault.alarm_id <= 0) {
				beeper_cnt = 0;
				recovery_cnt = 0;
				add_repeating_timer_ms(100, power_en_fault_callback, NULL, &timer_power_en_fault);
			}
		}
	}

	gpio_put(BMCU_POWER_EN, enable);
}


void gpio_callback(uint gpio, uint32_t events) {
    if (SHUTDOWN_REQ == gpio) {
        power_enable(0);
        printf("SHUTDOWN_REQ 0x%x\n", events);
    }
    else {
        printf("gpio=%d 0x%x\n", gpio, events);
    }
}

void jetson_init(void)
{
    gpio_init(SYS_5V_EN);
    gpio_set_dir(SYS_5V_EN, GPIO_OUT);
    gpio_disable_pulls(SYS_5V_EN);
    gpio_put(SYS_5V_EN, 1); // always on

    gpio_init(BMCU_POWER_EN);
    gpio_set_dir(BMCU_POWER_EN, GPIO_OUT);
    gpio_disable_pulls(BMCU_POWER_EN);
    gpio_put(BMCU_POWER_EN, 0);

    gpio_init(BMCU_PWR_BTN);
    gpio_set_dir(BMCU_PWR_BTN, GPIO_IN);
    gpio_pull_up(BMCU_PWR_BTN);

    gpio_init(SHUTDOWN_REQ);
    gpio_set_dir(SHUTDOWN_REQ, GPIO_IN);
    gpio_disable_pulls(SHUTDOWN_REQ);

    gpio_init(BMCU_ACOK);
    gpio_set_dir(BMCU_ACOK, GPIO_IN);
    gpio_pull_up(BMCU_ACOK);

    gpio_init(RESET_N);
    gpio_set_dir(RESET_N, GPIO_IN);
    gpio_disable_pulls(RESET_N);

    gpio_init(BEEPER);
    gpio_set_dir(BEEPER, GPIO_OUT);
    gpio_disable_pulls(BEEPER);
    gpio_put(BEEPER, 0);

    sleep_ms(PWR_STABLE_WAIT);
    gpio_set_irq_enabled_with_callback(SHUTDOWN_REQ,
        GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}


void jetson_auto_on(void)
{
    printf("BMCU_ACOK=%d\n", gpio_get(BMCU_ACOK));
    if (gpio_get(BMCU_ACOK)) {
        printf("POWER_EN\n");
        power_enable(1);
    }
}


bool power_btn_callback(struct repeating_timer *t)
{
    static int pwr_btn = 0; 		// must be static
    static int pwr_btn_pressed = 0; // must be static

    if (0 == gpio_get(BMCU_PWR_BTN)) {
        pwr_btn++;
    }
    else {
        pwr_btn = 0;
        pwr_btn_pressed = 0;
    }

    if (pwr_btn_pressed)
        return true;

    if (gpio_get(BMCU_POWER_EN)) {
        if (pwr_btn >= PWR_OFF_DELAY) {
            pwr_btn_pressed = 1;
            power_enable(0);
            printf("Power off!\n");
        }
    }
    else {
        if (pwr_btn >= PWR_ON_DELAY) {
            pwr_btn_pressed = 1;
            power_enable(1);
            printf("Power on!\n");
        }
    }

    return true;
}

void jetson_pwr_btn(void)
{
    static struct repeating_timer timer; // must be static
    add_repeating_timer_ms(TIMER_PWR_BTN, power_btn_callback, NULL, &timer);
}
