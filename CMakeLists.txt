cmake_minimum_required(VERSION 3.20.0)
set(BOARD stm32_min_dev/stm32f103xb CACHE STRING "Default board")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr-zl27arm)

list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
include(nanopb)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE
        src/main_nanopb.c
)
zephyr_nanopb_sources(app src/data.proto)