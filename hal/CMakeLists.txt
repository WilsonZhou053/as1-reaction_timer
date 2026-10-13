add_library(hal STATIC
    src/led.c
    src/joystick.c
)

target_include_directories(hal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hal PUBLIC Threads::Threads)
