/**
 * @file adc.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _TIDBIT_ADC_HEADER
#define _TIDBIT_ADC_HEADER_

/*
 * C HEADERS
 */

#include "hardware/adc.h"


typedef struct tb_adc {
    uint gpio;
    uint input;

    float r2v;
    uint16_t raw;
} tb_adc_t;

/**
 * @brief Setup a ADC with a conversion factor computed from a min and max value
 * 
 * @param gpio 
 * @param min  - minimum value
 * @param max  - maximum value
 * @return tb_adc_t 
 */
extern tb_adc_t tb_adc_setup(uint gpio, uint input, float min, float max);

/**
 * @brief Read a value and convert it to the range specified with setup
 * 
 * @param adc 
 * @return float 
 */
extern float tb_adc_read(tb_adc_t *adc);

#endif // _TIDBIT_ADC_HEADER_
