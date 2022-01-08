aux_source_directory(. DIR_OLED_SRCS)

#include_directories(../Config)

add_library(OLED ${DIR_OLED_SRCS})
target_link_libraries(OLED PUBLIC Config pico_stdlib hardware_spi hardware_i2c hardware_pwm)
