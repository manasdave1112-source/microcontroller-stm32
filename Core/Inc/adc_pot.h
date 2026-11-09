/*
 * adc_pot.h
 *
 *  Created on: 09-Nov-2026
 *      Author: manas
 */

#ifndef ADC_POT_H
#define ADC_POT_H

#include <stdint.h>

/*
 * Starts ADC1 (PA0 / IN1) free-running and sets up DMA so the samples
 * land in buf[0..len-1], 12-bit values (0..4095). DMA is circular: when it
 * reaches the end it wraps and overwrites the oldest samples.
 * len max 65535. Declare the array volatile, since DMA writes it behind the CPU's back.
 */
void adc_pot_init(volatile uint16_t *buf, uint32_t len);

#endif /* ADC_POT_H */
