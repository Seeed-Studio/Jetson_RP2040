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
    jetson_auto_on();
    jetson_pwr_btn();

    for (;;) {
    }

    return 0;
}
