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

    printf("RP2040 startup!!!\n");
    jetson_init();
    jetson_auto_on();
    jetson_pwr_btn();
    jetson_pwr_detect();

    for (;;) {
    }

    return 0;
}
