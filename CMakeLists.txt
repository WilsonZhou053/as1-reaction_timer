cmake_minimum_required(VERSION 3.18)
project(reaction_timer VERSION 1.0 DESCRIPTION "Reaction Timer Project" LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_ADDRESS_SANITIZER "Enable AddressSanitizer for host debugging" OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_definitions(_POSIX_C_SOURCE=200809L)

if(ENABLE_ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

find_package(Threads REQUIRED)

add_subdirectory(hal)
add_subdirectory(app)
