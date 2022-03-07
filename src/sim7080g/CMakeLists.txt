set(TIDBIT sim7080g)

add_executable(${TIDBIT}_main
        main.c
)

add_library(${TIDBIT} STATIC
        ${TIDBIT}_core.c
        ${TIDBIT}_network.c
        ${TIDBIT}_ssl.c
        ${TIDBIT}_tcpudp.c
        ${TIDBIT}_ntp.c
        ${TIDBIT}_http.c
        ${TIDBIT}_cmd.c
        ${TIDBIT}_sms.c
)

target_link_libraries(${TIDBIT}
        pico_stdlib
        pico_stdio
        pico_malloc
        hardware_gpio
        hardware_uart
        hardware_irq
        hardware_rtc

        uart_io
)

target_link_libraries(${TIDBIT}_main
        ${TIDBIT}
)

target_include_directories(${TIDBIT} PUBLIC
        ../uart_io
)

pico_enable_stdio_usb(${TIDBIT}_main 1)
pico_enable_stdio_uart(${TIDBIT}_main 0)
pico_add_extra_outputs(${TIDBIT}_main)