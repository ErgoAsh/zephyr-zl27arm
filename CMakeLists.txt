cmake_minimum_required(VERSION 3.20.0)
set(BOARD stm32_min_dev/blue/stm32f103xb CACHE STRING "Default board")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr-zl27arm)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE
        src/rtt_main.c
)
