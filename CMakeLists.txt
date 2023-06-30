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
    deps/pico-ssd1306/ssd1306.c
    deps/exceptions4c/lite/src/e4c_lite.c
    deps/hx711-pico-c/src/common.c
    deps/hx711-pico-c/src/hx711.c
    deps/hx711-pico-c/src/util.c)

#target_compile_definitions(mignon-grind-by-weight PRIVATE E4C_THREADSAFE)

target_include_directories(mignon-grind-by-weight PRIVATE deps/)
target_include_directories(mignon-grind-by-weight PRIVATE deps/exceptions4c/lite/src/)
target_include_directories(mignon-grind-by-weight PRIVATE deps/hx711-pico-c/include)
target_include_directories(mignon-grind-by-weight PRIVATE deps/pico-ssd1306)
target_include_directories(mignon-grind-by-weight PRIVATE include/)

target_link_libraries(mignon-grind-by-weight pico_stdlib hardware_dma hardware_pio hardware_i2c)

pico_add_extra_outputs(mignon-grind-by-weight)
