cmake_minimum_required(VERSION 3.12)

# Import the SDK
include(pico_sdk_import.cmake)

# Initialize some stuff
project(pico_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include_directories(./lib/OLED)
include_directories(./lib/GUI)
include_directories(./lib/Fonts)
include_directories(./lib/Config)
include_directories(./lib/ScreenUpdater)

# Add oled screen library
add_subdirectory(lib/Config)
add_subdirectory(lib/OLED)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
add_subdirectory(lib/ScreenUpdater)

add_executable(main main.c)
add_executable(wipe_flash wipe_flash.c)

pico_add_extra_outputs(main)
pico_add_extra_outputs(wipe_flash)

target_link_libraries(main OLED pico_stdlib hardware_spi GUI Fonts ScreenUpdate hardware_gpio m)
target_link_libraries(wipe_flash OLED pico_stdlib hardware_spi GUI Fonts hardware_flash hardware_sync)
