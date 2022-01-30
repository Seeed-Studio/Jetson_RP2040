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


void gpio_callback(uint gpio, uint32_t events) {
    if (SHUTDOWN_REQ == gpio) {
        gpio_put(BMCU_POWER_EN, 0);
        printf("SHUTDOWN_REQ 0x%x\n", events);
    }
    else {
        printf("gpio=%d 0x%x\n", gpio, events);
    }
}

void jetson_init(void)
{
    gpio_init(BMCU_PWR_BTN);
    gpio_set_dir(BMCU_PWR_BTN, GPIO_IN);
    gpio_pull_up(BMCU_PWR_BTN);

    gpio_init(SHUTDOWN_REQ);
    gpio_set_dir(SHUTDOWN_REQ, GPIO_IN);
    gpio_pull_up(SHUTDOWN_REQ);

    gpio_init(BMCU_ACOK);
    gpio_set_dir(BMCU_ACOK, GPIO_IN);
    gpio_pull_up(BMCU_ACOK);

    gpio_init(BMCU_POWER_EN);
    gpio_set_dir(BMCU_POWER_EN, GPIO_OUT);
    gpio_put(BMCU_POWER_EN, 0);

    gpio_init(RESET_N);
    gpio_set_dir(RESET_N, GPIO_IN);
    gpio_pull_up(RESET_N);

    sleep_ms(100);
    gpio_set_irq_enabled_with_callback(SHUTDOWN_REQ, 
        GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // adc
    adc_init();
    adc_gpio_init(ADC_5V);
    adc_gpio_init(ADC_3V);
}


void jetson_auto_on(void)
{
    printf("BMCU_ACOK=%d\n", gpio_get(BMCU_ACOK));
    if (gpio_get(BMCU_ACOK)) {
        printf("POWER_EN\n");
        gpio_put(BMCU_POWER_EN, 1);
    }
}


static int pwr_btn = 0;
static int pwr_state = 0;
bool power_btn_callback(struct repeating_timer *t)
{
    if (0 == gpio_get(BMCU_PWR_BTN)) {
        pwr_btn++;
    }
    else {
        pwr_btn = 0;
        pwr_state = 0;
    }

    if (pwr_state)
        return true;

    if (gpio_get(BMCU_POWER_EN)) {
        if (pwr_btn >= PWR_OFF_DELAY) {
            pwr_state = 1;
            gpio_put(BMCU_POWER_EN, 0);
            printf("Power off!\n");
        }
    }
    else {
        if (pwr_btn >= PWR_ON_DELAY) {
            pwr_state = 1;
            gpio_put(BMCU_POWER_EN, 1);
            printf("Power on!\n");
        }
    }

    return true;
}

struct repeating_timer timer_btn;
void jetson_pwr_btn(void)
{
    add_repeating_timer_ms(TIMER_PWR_BTN, power_btn_callback, NULL, &timer_btn);
}


bool power_detect_callback(struct repeating_timer *t)
{
    uint32_t v3, v5;

    adc_select_input(0);
    v5 = adc_read();
    v5 *= 3300;
    v5 /= 4096;
    adc_select_input(1);
    v3 = adc_read();
    v3 *= 3300;
    v3 /= 4096;

    if (gpio_get(BMCU_POWER_EN)) {
        if (v5 <= PWR_5V_LOWER) {
            gpio_put(BMCU_POWER_EN, 0);
            printf("Low volt off! %d,%d\n", v5, v3);
        }
    }

    return true;
}

struct repeating_timer timer_pwr;
void jetson_pwr_detect(void)
{
    add_repeating_timer_ms(TIMER_PWR_DETECT, power_detect_callback, NULL, &timer_pwr);
}
