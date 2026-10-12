#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "hal/led.h"
#include "hal/joystick.h"
#include "app_time.h"

int main(void)
{
    // Initialize random number generator
    srand((unsigned int)time(NULL));

    // Initialize hardware modules
    led_init();
    joystick_init();

    // Display welcome message and instructions
    printf("Hello embedded world, from Wilson (Si Hang) Zhou!\n");
    printf("When the LEDs light up, press the joystick in that direction!\n");
    printf("(Press left or right to exit)\n\n");

    long long best_time = 0; // Tracks the fastest correct response time

    while (true) {
        // Step 1: Print "get ready" and flash LEDs 4 times (Green 250ms, Red 250ms)
        printf("Get ready...\n");
        for (int i = 0; i < 4; i++) {
            led_greenOn(true);
            time_sleepForMs(250);
            led_greenOn(false);

            led_redOn(true);
            time_sleepForMs(250);
            led_redOn(false);
        }

        // Step 2: If joystick is currently pressed up/down, wait until released
        bool printed_let_go = false;
        while (joystick_readDirection() == joystickUp || joystick_readDirection() == joystickDown) {
            if (!printed_let_go) {
                printf("Please let go of joystick\n");
                printed_let_go = true;
            }
            time_sleepForMs(50);
        }

        // Step 3: Wait a random amount of time between 0.5s (500ms) and 3.0s (3000ms)
        long long random_delay = 500 + (rand() % 2501);
        time_sleepForMs(random_delay);

        // Step 4: If pressed too soon after delay, report and restart loop
        if (joystick_readDirection() == joystickUp || joystick_readDirection() == joystickDown) {
            printf("too soon\n\n");
            continue;
        }

        // Step 5: Pick random direction (Up or Down)
        int choice = rand() % 2; // 0 for Up, 1 for Down
        joystickDirection target_dir = (choice == 0) ? joystickUp : joystickDown;

        if (target_dir == joystickUp) {
            printf("Press UP now!\n");
            led_greenOn(true);
        } else {
            printf("Press DOWN now!\n");
            led_redOn(true);
        }

        // Step 6: Time how long it takes for the user to press the joystick (timeout after 5s)
        long long start_time = time_getTimeInMs();
        joystickDirection user_press = joystick_readDirection();

        while (user_press == joystickNone) {
            long long current_time = time_getTimeInMs();
            if ((current_time - start_time) > 5000) {
                printf("No input within 5000ms; quitting!\n");
                led_turnOffAll();
                led_cleanup();
                joystick_cleanup();
                return 0;
            }
            user_press = joystick_readDirection();
            time_sleepForMs(10); // Small poll delay to prevent CPU saturation
        }

        long long reaction_time = time_getTimeInMs() - start_time;
        led_turnOffAll(); // Turn off prompt LED immediately

        // Step 7: Process user's input
        if (user_press == joystickLeft || user_press == joystickRight) {
            printf("User selected to quit.\n");
            break;
        } else if (user_press == target_dir) {
            printf("Correct!\n");
            if (best_time == 0 || reaction_time < best_time) {
                best_time = reaction_time;
                printf("New best time!\n");
            }
            printf("Your reaction time was %lldms; best so far in game is %lldms.\n", reaction_time, best_time);

            // Flash green LED 5 times in 1 second (100ms on, 100ms off)
            for (int i = 0; i < 5; i++) {
                led_greenOn(true);
                time_sleepForMs(100);
                led_greenOn(false);
                time_sleepForMs(100);
            }
        } else {
            printf("Incorrect.\n");

            // Flash red LED 5 times in 1 second (100ms on, 100ms off)
            for (int i = 0; i < 5; i++) {
                led_redOn(true);
                time_sleepForMs(100);
                led_redOn(false);
                time_sleepForMs(100);
            }
        }

        printf("\n");
    }

    // Ensure clean shutdown and LEDs left off per requirements
    led_cleanup();
    joystick_cleanup();

    return 0;
}
