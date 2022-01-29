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

    for (;;) {
    }

    return 0;
}
