set(TIDBIT hd44780)

add_executable(${TIDBIT}_main
        main.c
)

add_library(${TIDBIT} STATIC
        ${TIDBIT}.c
)

target_link_libraries(${TIDBIT}
        pico_stdlib
        hardware_i2c
)

target_link_libraries(${TIDBIT}_main
        ${TIDBIT}
)


pico_add_extra_outputs(${TIDBIT}_main)