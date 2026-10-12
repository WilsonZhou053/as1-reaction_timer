# CMakeLists.txt for HAL
# Build a library (`hal`) which exposes the header files as "hal/*.h"
# Use header as: #include "hal/led.h", #include "hal/joystick.h", etc.

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")

add_library(hal STATIC ${MY_SOURCES})

target_include_directories(hal PUBLIC include)
