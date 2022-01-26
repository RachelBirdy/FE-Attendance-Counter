aux_source_directory(. DIR_GPIOBUTTONS)

include_directories(../Config)

add_library(GPIO_Buttons ${DIR_GPIOBUTTONS})
target_link_libraries(GPIO_Buttons hardware_gpio Structs Config)
