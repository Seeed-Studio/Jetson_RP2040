cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project($Jetson_RP2040)

include_directories(${PROJECT_SOURCE_DIR}/inc)

pico_sdk_init()

set(SRC_LIST main.c)
add_executable(Jetson_RP2040 ${SRC_LIST})

target_link_libraries(Jetson_RP2040
		pico_stdlib
	)

add_definitions(-DPICO_DEFAULT_UART=1
				-DPICO_DEFAULT_UART_RX_PIN=5
				-DPICO_DEFAULT_UART_TX_PIN=4)

pico_add_extra_outputs(Jetson_RP2040)
