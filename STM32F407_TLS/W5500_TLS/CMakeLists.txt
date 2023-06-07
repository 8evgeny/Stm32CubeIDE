cmake_minimum_required(VERSION 3.15)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set ARM compiler
set(TOOLS_PATH /home/evg/toolchain/gcc-arm-none-eabi-new)
#set(CMAKE_C_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
#set(CMAKE_CXX_LINKER_LAUNCHER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${TOOLS_PATH}/bin/arm-none-eabi-objcopy)

project(W5500_TLS C CXX ASM)

set (w5500 ${CMAKE_HOME_DIRECTORY}/TrueStudio/W5500_TLS)
set (ioDriver ${CMAKE_HOME_DIRECTORY}/ioLibrary_Driver)

#SET(MYCOMPILE_FLAGS '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"') //для mbedTLS
add_definitions(${MYCOMPILE_FLAGS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}\
    -mcpu=cortex-m4 \
    -std=gnu11 \
    -DUSE_HAL_DRIVER \
    -DSTM32F407xx \
    -c \
    -Os \
    -ffunction-sections \
    -fdata-sections \
    -Wall \
    -fstack-usage \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -mthumb \
    -Xlinker -Map=${PROJECT_NAME}.map \
    ")
#    --specs=nano.specs \

include_directories(
    ${w5500}/Inc
    ${w5500}/Drivers/CMSIS/Include
    ${w5500}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${w5500}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${w5500}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${w5500}/Drivers/TLS
    ${w5500}/Drivers/TLS/mbedtls
    ${w5500}/Drivers/TLS/mbedtls/include
    ${ioDriver}
    ${ioDriver}/Ethernet
    ${ioDriver}/Ethernet/W5500
    )

file (GLOB w5500_src
    ${w5500}/Drivers/STM32F4xx_HAL_Driver/Src/*.c
    ${w5500}/Drivers/TLS/*.c
    ${w5500}/Drivers/TLS/mbedtls/library/*.c
)
add_library(W5500_LIB ${w5500_src})

file (GLOB ioDriver_src
    ${ioDriver}/Ethernet/*.c
    ${ioDriver}/Ethernet/W5500/*.c
)
add_library(ioDriver_LIB ${ioDriver_src})

file (GLOB src ${w5500}/Src/*.c ${w5500}/startup/startup_stm32f407vgtx.s)
add_executable(${PROJECT_NAME}.elf ${src})

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE CXX)
set(LINKER_SCRIPT_FLASH "${w5500}/STM32F407VGTX_FLASH.ld")
set(LINKER_FLAGS "SHELL: \
    -T${LINKER_SCRIPT_FLASH} \
    -mcpu=cortex-m4 \
    --specs=nosys.specs \
    -Wl,--gc-sections \
    -static \
    --specs=nano.specs \
    -mfpu=fpv5-d16 \
    -mfloat-abi=hard \
    -mthumb
    -Wl,--start-group \
    -lc \
    -lm \
    -Wl,--end-group \
    ")

target_link_options(${PROJECT_NAME}.elf PRIVATE ${LINKER_FLAGS})

target_link_libraries(${PROJECT_NAME}.elf PUBLIC
    W5500_LIB
    ioDriver_LIB
    )

set(HEX_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMENT "Building ${HEX_FILE} ")

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${BIN_FILE} ")

#add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
#    COMMAND st-flash erase
#    COMMENT "Erasing . . .")

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND st-flash --reset write ${BIN_FILE} 0x8000000
    COMMENT "Write Flash ")
