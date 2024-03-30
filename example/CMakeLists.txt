cmake_minimum_required(VERSION 3.13)

set(NAME pico_st7789_example)
#set(URL "https://github.com/dcooperdalrymple/pico-neotrellis/main/example")

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
#set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

project(${NAME} C CXX ASM)

# Add executable. Default name is the project name,
add_executable(${NAME} pico_st7789_example.cpp)
add_subdirectory(../pico_st7789 build)

target_link_libraries(${NAME} pico_stdlib pico_st7789)
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 1)

#pico_set_program_url(${NAME} ${URL})
