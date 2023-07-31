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
    src/estimators/linear_fitting.c
    src/estimators/median.c
    deps/exceptions4c/lite/src/e4c_lite.c
    deps/hx711-pico-c/src/common.c
    deps/hx711-pico-c/src/hx711.c
    deps/hx711-pico-c/src/util.c
    src/estimators/sample_buffer/sample_buffer.c
    src/user_interface/button.c
    src/pages/main_page.c
    src/scale.c src/config.c src/config.h include/mignon-grind-by-weight/defs.h src/user_interface/app.c src/user_interface/app.h)

target_include_directories(mignon-grind-by-weight PRIVATE deps/)
target_include_directories(mignon-grind-by-weight PRIVATE deps/exceptions4c/lite/src/)
target_include_directories(mignon-grind-by-weight PRIVATE deps/hx711-pico-c/include)
target_include_directories(mignon-grind-by-weight PRIVATE include/)

target_link_libraries(mignon-grind-by-weight pico_stdlib hardware_dma hardware_pio hardware_i2c hardware_flash)

pico_add_extra_outputs(mignon-grind-by-weight)
