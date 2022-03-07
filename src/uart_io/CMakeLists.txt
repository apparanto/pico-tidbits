set(TIDBIT uart_io)

add_executable(${TIDBIT}_main
        main.c
)

add_library(${TIDBIT} STATIC
        ${TIDBIT}.c
)

target_link_libraries(${TIDBIT}
        pico_stdlib
        pico_stdio
        pico_malloc
        hardware_gpio
        hardware_uart
        hardware_rtc
        hardware_timer
)

target_link_libraries(${TIDBIT}_main
        ${TIDBIT}
)

pico_enable_stdio_usb(${TIDBIT}_main 1)
pico_enable_stdio_uart(${TIDBIT}_main 0)
pico_add_extra_outputs(${TIDBIT}_main)