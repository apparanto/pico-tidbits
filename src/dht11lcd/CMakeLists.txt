set(GOODY dht11lcd)

add_executable(${GOODY}
        main.c
)

# pull in common dependencies
target_link_libraries(${GOODY} 
        dht11
        hd44780
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${GOODY})
