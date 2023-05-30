cmake_minimum_required(VERSION 3.15)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set ARM compiler
set(TOOLS_PATH /home/evg/toolchain/gcc-arm-none-eabi-new)
#set(CMAKE_C_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
#set(CMAKE_CXX_LINKER_LAUNCHER ${TOOLS_PATH}/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${TOOLS_PATH}/bin/arm-none-eabi-objcopy)

#Закомментировать при прошивке DT
#add_definitions(-DINTRON)

#Режим http / https
add_definitions(-DTLS_ON)

project(11_Converter_F407_INTRON_DT C CXX ASM)

#set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_BUILD_TYPE release)

set (w5500 ${CMAKE_HOME_DIRECTORY}/ioLibrary_Driver/Ethernet)
set (fatfs ${CMAKE_HOME_DIRECTORY}/Middlewares/Third_Party/FatFs/src)
#set (littlefs ${CMAKE_HOME_DIRECTORY}/Middlewares/Third_Party/littlefs_new)
set (littlefs ${CMAKE_HOME_DIRECTORY}/Middlewares/Third_Party/littlefs)
set (wolfssl ${CMAKE_HOME_DIRECTORY}/Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl)

#SET(MYCOMPILE_FLAGS '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"') //для mbedTLS
add_definitions(${MYCOMPILE_FLAGS})
add_definitions("-DUSE_FULL_LL_DRIVER")

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
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    Core/Inc
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/CMSIS/Core/Include
    Drivers/CMSIS/Include
    ${w5500}
    ${w5500}/W5500
    FATFS/App
    FATFS/Target
    ${fatfs}
    ${littlefs}
    wolfSSL
    ${wolfssl}
    ${wolfssl}/wolfssl
    wolfssl-examples-master/embedded
    )

file (GLOB hal_src Drivers/STM32F4xx_HAL_Driver/Src/*.c)
add_library(hal_LIB ${hal_src})

file (GLOB w5500_src ${w5500}/*.c ${w5500}/W5500/w5500.c)
add_library(w5500_LIB ${w5500_src})

file (GLOB fatfs_src FATFS/App/*.c FATFS/Target/*.c ${fatfs}/*.c ${fatfs}/option/*.c)
add_library(fatfs_LIB ${fatfs_src})

file (GLOB littlefs_src ${littlefs}/*.c ${littlefs}/*.h)
add_library(littlefs_LIB ${littlefs_src})

file (GLOB wolfssl_src ${wolfssl}/src/*.c
    ${wolfssl}/wolfcrypt/src/*.c
    wolfSSL/wolfssl_example.c
    wolfssl-examples-master/embedded/tls-client-server.c
    wolfssl-examples-master/embedded/tls-server-size.c
#    wolfssl-examples-master/embedded/tls-sock-server.c
    )
add_library(wolfssl_LIB ${wolfssl_src})

file (GLOB src Core/Src/*.c Core/Startup/startup_stm32f407vgtx.s)
add_executable(${PROJECT_NAME}.elf ${src})

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

target_link_libraries(${PROJECT_NAME}.elf PUBLIC
    hal_LIB
    w5500_LIB
    fatfs_LIB
    littlefs_LIB
    wolfssl_LIB
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
