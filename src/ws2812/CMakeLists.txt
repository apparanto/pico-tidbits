set(TIDBIT ws2812)

add_executable(${TIDBIT}_main
        main.c
)

add_library(${TIDBIT} STATIC
        ${TIDBIT}.c
)

target_link_libraries(${TIDBIT}
        pico_stdlib
        hardware_pio
        hardware_clocks
)

target_link_libraries(${TIDBIT}_main
        ${TIDBIT}
)

pico_generate_pio_header(${TIDBIT}_main
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

pico_add_extra_outputs(${TIDBIT}_main)

