#include "hal/joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev0.0"

// Define your MCP3208 channel numbers based on your wiring (e.g., CH0 and CH1)
#define X_AXIS_CHANNEL 0
#define Y_AXIS_CHANNEL 1

static bool is_initialized = false;
static int spi_fd = -1;

// Calibration variables (start with initial default bounds, updated dynamically)
static int x_min = 0;
static int x_max = 4095;
static int y_min = 0;
static int y_max = 4095;
static bool calibration_initialized = false;

// Helper function to read raw ADC value via SPI from the MCP3208
static int read_spi_adc(uint8_t channel) {
    if (spi_fd < 0) return 0;

    // MCP3208 Single-ended mode protocol frame
    uint8_t tx[] = {
        0x06 | ((channel & 0x04) >> 2),
        (channel & 0x03) << 6,
        0x00
    };
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = 1000000,
        .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI read error");
        return 0;
    }

    int result = ((rx[1] & 0x0F) << 8) | rx[2];
    return result;
}

void joystick_init(void)
{
    assert(!is_initialized);

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        perror("Failed to open SPI device for Joystick");
    }

    is_initialized = true;
    calibration_initialized = false;
}

void joystick_cleanup(void)
{
    assert(is_initialized);

    if (spi_fd >= 0) {
        close(spi_fd);
        spi_fd = -1;
    }

    is_initialized = false;
}

joystickDirection joystick_readDirection(void)
{
    assert(is_initialized);

    int x_val = read_spi_adc(X_AXIS_CHANNEL);
    int y_val = read_spi_adc(Y_AXIS_CHANNEL);

    // Auto-calibration: update min and max dynamically as the joystick moves[cite: 4]
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

    // Define threshold: require user to push most of the way (e.g., 30% from center)[cite: 4]
    int x_threshold = x_range * 0.30;
    int y_threshold = y_range * 0.30;

    // Handle edge case where range is very small initially
    if (x_range < 100) x_threshold = 500;
    if (y_range < 100) y_threshold = 500;

    // Evaluate directions
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
