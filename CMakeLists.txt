cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(cmakes/pico_sdk_import.cmake)

project(mignon-grind-by-weight)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(mignon-grind-by-weight
    src/main.c
    deps/exceptions4c/lite/src/e4c_lite.c)

target_include_directories(mignon-grind-by-weight PRIVATE deps/)

target_link_libraries(mignon-grind-by-weight pico_stdlib)

pico_add_extra_outputs(mignon-grind-by-weight)
