#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jetson.h"


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

    printf("%09d\r", pwr_btn);
    if (gpio_get(BMCU_POWER_EN)) {
        if (pwr_btn >= PWR_OFF_DELAY) {
            pwr_state = 1;
            gpio_put(BMCU_POWER_EN, 0);
            printf("\npower off\n");
        }
    }
    else {
        if (pwr_btn >= PWR_ON_DELAY) {
            pwr_state = 1;
            gpio_put(BMCU_POWER_EN, 1);
            printf("\npower on\n");
        }
    }

    return true;
}

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
}

int main()
{
    stdio_init_all();

    printf("RP2040 startup!!!\n");
    jetson_init();

    printf("BMCU_ACOK=%d\n", gpio_get(BMCU_ACOK));
    if (gpio_get(BMCU_ACOK)) {
        printf("POWER_EN\n");
        gpio_put(BMCU_POWER_EN, 1);
    }

    struct repeating_timer timer;
    add_repeating_timer_ms(TIMER_DELAY, power_btn_callback, NULL, &timer);

    for (;;) {
    }

    return 0;
}
