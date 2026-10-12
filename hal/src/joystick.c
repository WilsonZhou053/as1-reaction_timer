#include "hal/joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

static bool is_initialized = false;

// Define your ADC channel numbers based on your wiring (e.g., AIN4 and AIN5)
#define X_AXIS_CHANNEL 4
#define Y_AXIS_CHANNEL 5

// Calibration variables (start with reasonable initial default bounds)
static int x_min = 0;
static int x_max = 4095;
static int y_min = 0;
static int y_max = 4095;
static bool calibration_initialized = false;

// Helper function to read raw ADC value from sysfs
static int read_adc(int channel) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/bus/iio/devices/iio:device0/in%d_input", channel);
    
    FILE *f = fopen(path, "r");
    if (!f) {
        // If file cannot be opened, return 0 or handle error gracefully
        return 0;
    }
    
    int val = 0;
    if (fscanf(f, "%d", &val) != 1) {
        val = 0;
    }
    fclose(f);
    return val;
}

void joystick_init(void)
{
    is_initialized = true;
    calibration_initialized = false;
}

void joystick_cleanup(void)
{
    assert(is_initialized);
    is_initialized = false;
}

joystickDirection joystick_readDirection(void)
{
    assert(is_initialized);

    int x_val = read_adc(X_AXIS_CHANNEL);
    int y_val = read_adc(Y_AXIS_CHANNEL);

    // Auto-calibration: update min and max dynamically as the joystick moves[cite: 3]
    if (!calibration_initialized) {
        x_min = x_max = x_val;
        y_min = y_max = y_val;
        calibration_initialized = true;
    } else {
        if (x_val < x_min) x_min = x_val;
        if (x_val > x_max) x_max = x_val;
        if (y_val < y_min) y_min = y_val;
        if (y_val > y_max) y_max = y_val;
    }

    // Calculate center and ranges
    int x_range = x_max - x_min;
    int y_range = y_max - y_min;
    int x_center = x_min + (x_range / 2);
    int y_center = y_min + (y_range / 2);

    // Define threshold: require user to push most of the way (e.g., 70% from center)[cite: 3]
    int x_threshold = x_range * 0.30;
    int y_threshold = y_range * 0.30;

    // Evaluate directions (adjust axis mapping depending on your physical wiring)
    if (y_val < (y_center - y_threshold)) {
        return joystickUp;
    } else if (y_val > (y_center + y_threshold)) {
        return joystickDown;
    } else if (x_val < (x_center - x_threshold)) {
        return joystickLeft;
    } else if (x_val > (x_center + x_threshold)) {
        return joystickRight;
    }

    return joystickNone;
}
