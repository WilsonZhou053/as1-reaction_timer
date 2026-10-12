#ifndef LED_H
#define LED_H
#include <stdbool.h>

void led_init(void);

//Turning led on and off
void led_greenOn(bool);
void led_redOn(bool);

void led_turnOffAll(void);

void led_cleanup(void);

#endif
