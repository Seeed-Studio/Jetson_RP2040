/**
 * Copyright (c) 2022 Seeed technology inc.
 *
 * The MIT License (MIT)
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jetson.h"

int main()
{
    stdio_init_all();

    printf("\nRP2040 startup!!!\n");
    printf("Version: %d.%d\n", (JETSON_VER>>4)&0x0F, (JETSON_VER)&0x0F);
    printf("Build: %s %s\n", __DATE__, __TIME__);

    jetson_init();
	jetson_wait_5v();
    jetson_auto_on();
    jetson_pwr_btn();

    for (;;) {
#if 0
        gpio_put(TEST_POINT, 1);
        for(int i = 0; i < 500; i++);
        gpio_put(TEST_POINT, 0);
        for(int j = 0; j < 500; j++);
#endif
    }

    return 0;
}
