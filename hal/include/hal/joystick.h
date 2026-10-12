#ifndef JOYSTICK_H
#define JOYSTICK_H

typedef enum {
    joystickUp,
    joystickDown,
    joystickLeft,
    joystickRight,
    joystickNone,

} joystickDirection;

void joystick_init(void);
void joystick_cleanup(void);

joystickDirection joystick_readDirection(void);

#endif