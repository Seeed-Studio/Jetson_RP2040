/**
 * Copyright (c) 2022 Seeed technology inc.
 *
 * The MIT License (MIT)
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "jetson.h"


static struct repeating_timer timer_power_en_fault = { 0 }; // must be static
static unsigned int beeper_cnt = 0, recovery_cnt = 0;
static bool power_en_fault_callback(struct repeating_timer *t)
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
	DBG_PRINT("%d\n", enable);
	if (enable) {
		if (!gpio_get(SHUTDOWN_REQ)) { // fault
			DBG_PRINT("SHUTDOWN_REQ low\n");
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

void jetson_init(void)
{
    gpio_init(SYS_5V_EN);
    gpio_set_dir(SYS_5V_EN, GPIO_OUT);
    gpio_disable_pulls(SYS_5V_EN);
    gpio_put(SYS_5V_EN, 1); // always on

    gpio_init(BMCU_PWR_BTN);
    gpio_set_dir(BMCU_PWR_BTN, GPIO_IN);
    gpio_pull_up(BMCU_PWR_BTN);

    gpio_init(SHUTDOWN_REQ);
    gpio_set_dir(SHUTDOWN_REQ, GPIO_IN);
    gpio_disable_pulls(SHUTDOWN_REQ);

    gpio_init(BMCU_ACOK);
    gpio_set_dir(BMCU_ACOK, GPIO_IN);
    gpio_pull_up(BMCU_ACOK);

    gpio_init(BMCU_POWER_EN);
    gpio_set_dir(BMCU_POWER_EN, GPIO_OUT);
    gpio_disable_pulls(BMCU_POWER_EN);
    gpio_put(BMCU_POWER_EN, 0);

	gpio_init(PWR_1V8_EN);
    gpio_set_dir(PWR_1V8_EN, GPIO_OUT);
    gpio_disable_pulls(PWR_1V8_EN);

    gpio_init(SATA_PwrEN2);
    gpio_set_dir(SATA_PwrEN2, GPIO_OUT);
    gpio_disable_pulls(SATA_PwrEN2);

    gpio_init(RESET_N);
    gpio_set_dir(RESET_N, GPIO_IN);
    gpio_disable_pulls(RESET_N);

    gpio_init(BEEPER);
    gpio_set_dir(BEEPER, GPIO_OUT);
    gpio_disable_pulls(BEEPER);
    gpio_put(BEEPER, 0);

	// adc
    adc_init();
    adc_gpio_init(ADC_5V_GPIO);
    adc_gpio_init(ADC_3V_GPIO);
    adc_gpio_init(ADC_12V_GPIO);

    sleep_ms(PWR_STABLE_WAIT);
}


static bool adc_5v_fault_callback(struct repeating_timer *t)
{
	// beeper
	static uint32_t beeper = 0; // must be static

	beeper++;
	if (beeper < 4) {
		gpio_put(BEEPER, !!(beeper&0x01));
	}
	else {
		gpio_put(BEEPER, 0);
		if (beeper >= 10) {
			beeper = 0;
		}
	}
	
	return true;
}

void jetson_wait_5v(void)
{
	uint16_t val = 0;
	uint32_t cnt = 0;
	static struct repeating_timer timer = { 0 }; // must be static

	adc_select_input(ADC_5V_CH);
	val = adc_read();
	if (val < PWR_5V_LOW)
	{
		add_repeating_timer_ms(100, adc_5v_fault_callback, NULL, &timer);
		while (1)
		{
			val = adc_read();
			if (val >= PWR_5V_LOW)
			{
				cnt++;
				if (cnt >= 10)
				{
					break;
				}
			}
			else
			{
				cnt = 0;
			}
			printf("5v detect raw = %d\n", val);
			sleep_ms(10);
		}
		cancel_repeating_timer(&timer);
		gpio_put(BEEPER, 0);
	}
	printf("5v detect OK raw = %d\n", val);
}

static bool adc_12v_fault_callback(struct repeating_timer *t)
{
	// beeper
	static uint32_t beeper = 0; // must be static

	beeper++;
	if (beeper < 15) {
		gpio_put(BEEPER, !!(beeper&0x01));
	}
	else {
		gpio_put(BEEPER, 0);
		if (beeper >= 30) {
			beeper = 0;
		}
	}

	return true;
}

void jetson_wait_12v(void)
{
	uint16_t val = 0;
	uint32_t cnt = 0;
	static struct repeating_timer timer = { 0 }; // must be static

	adc_select_input(ADC_12V_CH);
	val = adc_read();

	if (val < PWR_12V_LOW)
	{
		add_repeating_timer_ms(100, adc_12v_fault_callback, NULL, &timer);
		while (1)
		{
			val = adc_read();
			if (val >= PWR_12V_LOW)
			{
				cnt++;
				if (cnt >= 10)
				{
//					printf("Enable SATA power supply\n", val);
//					gpio_put(SATA_PwrEN2, 1);	//enable SATA power supply
					break;
				}
			}
			else
			{
				cnt = 0;
			}

			printf("12v detect raw = %d\n", val);
			sleep_ms(10);
		}

		cancel_repeating_timer(&timer);
		gpio_put(BEEPER, 0);
	}

//	printf("Enable SATA power supply, raw = %d\n", val);
//	gpio_put(SATA_PwrEN2, 1);	//enable SATA power supply

}

void jetson_auto_on(void)
{
    DBG_PRINT("BMCU_ACOK=%d\n", gpio_get(BMCU_ACOK));
    if (gpio_get(BMCU_ACOK)) {
        power_enable(1);
    }
}

static bool power_btn_callback(struct repeating_timer *t)
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
        }
    }
    else {
        if (pwr_btn >= PWR_ON_DELAY) {
            pwr_btn_pressed = 1;
            power_enable(1);
        }
    }

    return true;
}

static void gpio_callback(uint gpio, uint32_t events)
{
    if (SHUTDOWN_REQ == gpio)
	{
		for(int i = 0; i < 10; i++);		//delay

		if(0 == gpio_get(SHUTDOWN_REQ))
		{
			for(int j = 0; j < 10; j++);	//delay

			if(0 == gpio_get(SHUTDOWN_REQ))
			{
				power_enable(0);
				DBG_PRINT("SHUTDOWN_REQ\n");
			}
		}
	}
	else if(RESET_N == gpio)
	{
		for(int i = 0; i < 10; i++);		//delay
		if(0 == gpio_get(RESET_N))
		{
			gpio_put(PWR_1V8_EN, 0);
		}
		else
		{
			gpio_put(PWR_1V8_EN, 1);
		}
	}
	else
	{
		DBG_PRINT("gpio=%d 0x%x\n", gpio, events);
	}
}

void jetson_pwr_btn(void)
{
    static struct repeating_timer timer; // must be static
    add_repeating_timer_ms(TIMER_PWR_BTN, power_btn_callback, NULL, &timer);
    gpio_set_irq_enabled_with_callback(SHUTDOWN_REQ, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

	gpio_set_irq_enabled_with_callback(RESET_N, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}
