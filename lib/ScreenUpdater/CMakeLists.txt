aux_source_directory(. DIR_SCREEN_UPDATE)

add_library(ScreenUpdate ${DIR_SCREEN_UPDATE})
target_link_libraries(ScreenUpdate OLED hardware_spi GUI Fonts pico_stdlib)
