cmake_minimum_required(VERSION 3.15)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#Будет один код для 2-х плат F407 адрес будет меняться в CMake

# Set ARM compiler
set(TOOLS_PATH /home/evg/toolchain/gcc-arm-none-eabi-new)
#set(CMAKE_C_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
#set(CMAKE_CXX_LINKER_LAUNCHER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${TOOLS_PATH}/bin/arm-none-eabi-objcopy)

#Закомментировать при прошивке DT
#add_definitions(-DINTRON)

project(11_Converter_F407_INTRON_DT C CXX ASM)

#set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_BUILD_TYPE release)

SET(MYCOMPILE_FLAGS '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"')
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
    USB_HOST/App
    USB_HOST/Target
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    Core/Inc
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/CMSIS/Core/Include
    Drivers/CMSIS/Include
    ioLibrary_Driver/Ethernet
    ioLibrary_Driver/Ethernet/W5500
    FATFS/App
    FATFS/Target
    Middlewares/Third_Party/FatFs/src
    MBEDTLS/App
    Middlewares/Third_Party/mbedTLS/include
    )
file (GLOB SOURCES_C
    Core/Src/*.c
    Core/Startup/startup_stm32f407vgtx.s
    Drivers/STM32F4xx_HAL_Driver/Src/*.c
    ioLibrary_Driver/Ethernet/*.c
    ioLibrary_Driver/Ethernet/W5500/w5500.c
    FATFS/App/*.c
    FATFS/Target/*.c
    Middlewares/Third_Party/FatFs/src/*.c
    Middlewares/Third_Party/FatFs/src/option/*.c
    MBEDTLS/App/*.c
    Middlewares/Third_Party/mbedTLS/library/*.c
    MBEDTLS/Target/*.c
    )


add_executable(${PROJECT_NAME}.elf
    ${SOURCES_C}
    )

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE CXX)
set(LINKER_SCRIPT_FLASH "${CMAKE_SOURCE_DIR}/STM32F407VGTX_FLASH.ld")
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


#target_link_libraries(${PROJECT_NAME}.elf PUBLIC
#    )

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
