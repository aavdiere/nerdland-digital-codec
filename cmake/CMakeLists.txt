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
    ../app/src/core/system.c
    ../app/src/core/uart.c
    ../app/src/video/vga.c
    ../app/src/video/graphics.c
    ../app/src/video/font.c
)

target_include_directories(${BINARY} PRIVATE
    ${OPENCM3_DIR}/include
    ../app/include
)

target_compile_definitions(${BINARY} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    ${MCU_FAMILY}
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
