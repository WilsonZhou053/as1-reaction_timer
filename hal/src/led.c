#include "hal/led.h"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

static bool is_initialized = false;

static void write_led_file(const char *path, bool on)
{
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "%d", on ? 1 : 0);
    fclose(f);
}

void led_init(void)
{
    is_initialized = true;
    led_turnOffAll();
}

void led_greenOn(bool on)
{
    assert(is_initialized);
    write_led_file("/sys/class/leds/ACT/brightness", on);
}

void led_redOn(bool on)
{
    assert(is_initialized); 
    write_led_file("/sys/class/leds/PWR/brightness", on);
}

void led_turnOffAll(void)
{
    assert(is_initialized);
    led_greenOn(false);
    led_redOn(false);
}

void led_cleanup(void)
{
    assert(is_initialized); 
    led_turnOffAll();
    is_initialized = false;
}
