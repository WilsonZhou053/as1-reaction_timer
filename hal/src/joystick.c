#include "hal/joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev0.0"

#define X_AXIS_CHANNEL 0
#define Y_AXIS_CHANNEL 1

static bool is_initialized = false;
static int spi_fd = -1;

static int x_min = 0;
static int x_max = 4095;
static int y_min = 0;
static int y_max = 4095;

static int read_spi_adc(uint8_t channel) {
    if (spi_fd < 0) return 0;

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


    if (x_val < x_min) x_min = x_val;
    if (x_val > x_max) x_max = x_val;
    if (y_val < y_min) y_min = y_val;
    if (y_val > y_max) y_max = y_val;

    int x_range = x_max - x_min;
    int y_range = y_max - y_min;
    int x_center = x_min + (x_range / 2);
    int y_center = y_min + (y_range / 2);

    // Ensure most of lever is pushed
    int x_threshold = x_range * 0.30;
    int y_threshold = y_range * 0.30;

    // Direction Evaluation
    if (y_val < (y_center - y_threshold)) {
        return joystickDown;
    } else if (y_val > (y_center + y_threshold)) {
        return joystickUp;
    } else if (x_val < (x_center - x_threshold)) {
        return joystickLeft;
    } else if (x_val > (x_center + x_threshold)) {
        return joystickRight;
    }

    return joystickNone;
}
