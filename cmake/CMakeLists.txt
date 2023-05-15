cmake_minimum_required(VERSION 3.12)

################################################################################
# Customize
if(NOT DEFINED BINARY)
    set(BINARY stm32-vga)
endif()
message("Project name: ${BINARY}")

set(OPENCM3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/libopencm3")

# Build type: Debug, Release
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

################################################################################
# Export CMake commands to load into Visual Studio Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# Setup project
project(${BINARY})

################################################################################
# ARM and MCU specific setup
include(./toolchain-arm-none-eabi.cmake)
include(./stm32l4.cmake)

################################################################################
# Linker setup
string(TOLOWER ${MCU_FAMILY} MCU_LIBNAME)

################################################################################
# Files
add_executable(${BINARY}
    ../app/src/firmware.c
    ../app/src/core/syscalls.c
    ../app/src/core/sysmem.c
    ../app/src/core/system.c
    ../app/src/core/uart.c
    ../app/src/video/vga.c
    ../app/src/video/graphics.c
    ../app/src/video/font.c
    ../app/src/usb/usb.c
    ../app/src/usb/usbh_conf.c
    ../app/src/usb/usb_host.c
    ../app/src/usb/usbh_platform.c
    ../app/src/usb/usbh_core.c
    ../app/src/usb/usbh_ctlreq.c
    ../app/src/usb/usbh_hid.c
    ../app/src/usb/usbh_hid_keybd.c
    ../app/src/usb/usbh_hid_mouse.c
    ../app/src/usb/usbh_hid_parser.c
    ../app/src/usb/usbh_ioreq.c
    ../app/src/usb/usbh_pipes.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_hcd.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    ../vendor/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_usb.c 
)

target_include_directories(${BINARY} PRIVATE
    ${OPENCM3_DIR}/include
    ../vendor/STM32L4xx_HAL_Driver/Inc
    ../vendor/STM32L4xx_HAL_Driver/Inc/Legacy
    ../vendor/CMSIS/Device/ST/STM32L4xx/Include
    ../vendor/CMSIS/Include
    ../app/include
)

target_compile_definitions(${BINARY} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    STM32L4
    STM32L476xx
    USE_HAL_DRIVER
    AZERTY_KEYBOARD
)

target_compile_options(${BINARY} PRIVATE
    -MD
    -Wundef
    $<$<COMPILE_LANGUAGE:CXX>:
        -Wconversion
        -Wno-volatile
        -Wold-style-cast
        -Wuseless-cast
        -Wsuggest-override
        -Weffc++
        -Wredundant-decls
    >
)

target_link_libraries(${BINARY} PRIVATE
    -L${OPENCM3_DIR}/lib
    -lopencm3_${MCU_LIBNAME}
)

print_section_sizes(${BINARY})
create_hex_output(${BINARY})
create_bin_output(${BINARY})
