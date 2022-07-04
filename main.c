/**
 * Copyright (c) 2022 Seeed technology inc.
 *
 * The MIT License (MIT)
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jetson.h"

static bool detect_5v_cb(struct repeating_timer *t)
{
    printf("detect_5v_cb\n");
    jetson_wait_5v();
//    jetson_wait_12v();
}

int main()
{
    stdio_init_all();

    printf("\nRP2040 startup!!!\n");
    printf("Version: %d.%d\n", (JETSON_VER>>4)&0x0F, (JETSON_VER)&0x0F);
    printf("Build: %s %s\n", __DATE__, __TIME__);

    jetson_init();
	jetson_wait_5v();
    jetson_wait_12v();
    jetson_auto_on();
    jetson_pwr_btn();

    static struct repeating_timer detect_timer = { 0 }; // must be static
    add_repeating_timer_ms(1000, detect_5v_cb, NULL, &detect_timer);

    for (;;) {
    }

    return 0;
}
