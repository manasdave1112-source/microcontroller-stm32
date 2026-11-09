/*
 * adc_pot.c
 *
 *  Created on: 09-Nov-2026
 *      Author: manas
 */
/*
 * Potentiometer ADC driver, STM32G474RET6, register level (CMSIS only).
 *
 * Wiring : pot ends to 3V3 and GND, wiper to PA0 (ADC1_IN1).
 *          A 100 nF cap from wiper to GND helps a lot.
 * Assumes: HCLK = 170 MHz already set up (CubeMX clock tree).
 * Flow   : ADC1 free-runs -> DMA1 Ch1 writes each result into your array.
 *          No interrupts, the CPU never touches the ADC after init.
 */
#include "adc_pot.h"
#include "stm32g4xx.h"

#define POT_ADC_CHANNEL   1u   /* PA0 = ADC1_IN1 */
#define DMAMUX_REQ_ADC1   5u   /* DMAMUX request ID for ADC1 */

static void delay_loops(uint32_t n)
{
    while (n--) { __NOP(); }
}

void adc_pot_init(volatile uint16_t *buf, uint32_t len)
{
    /* 1. Clocks */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_ADC12EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN;
    (void)RCC->AHB1ENR;                         /* readback = small delay */

    /* 2. PA0 to analog mode */
    GPIOA->MODER |= GPIO_MODER_MODE0;           /* 0b11 = analog */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;

    /* 3. ADC clock = HCLK/4 (42.5 MHz, under the 60 MHz limit) */
    ADC12_COMMON->CCR = (ADC12_COMMON->CCR & ~ADC_CCR_CKMODE)
                      | (3u << ADC_CCR_CKMODE_Pos);

    /* 4. Wake the ADC: leave deep power-down, start the regulator */
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;
    delay_loops(10000);                         /* needs >= 20 us */

    /* 5. Calibrate (single-ended) */
    ADC1->CR &= ~ADC_CR_ADCALDIF;
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL) { }
    delay_loops(100);

    /* 6. Config: 12-bit, right aligned, continuous, DMA circular */
    ADC1->CFGR |= ADC_CFGR_CONT | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG;

    /* 16x hardware oversampling, shift right 4 -> still a 12-bit result */
    ADC1->CFGR2 = ADC_CFGR2_ROVSE
                | (3u << ADC_CFGR2_OVSR_Pos)    /* 0b011 = 16x */
                | (4u << ADC_CFGR2_OVSS_Pos);   /* shift by 4  */

    /* Long sample time (247.5 cycles) so a high-impedance pot settles */
    ADC1->SMPR1 = (6u << ADC_SMPR1_SMP1_Pos);

    /* Sequence: 1 conversion, channel 1 */
    ADC1->SQR1 = (0u << ADC_SQR1_L_Pos)
               | (POT_ADC_CHANNEL << ADC_SQR1_SQ1_Pos);

    /* 7. DMA1 Ch1: ADC1->DR -> buf, 16-bit, circular, no interrupts */
    DMAMUX1_Channel0->CCR = DMAMUX_REQ_ADC1;    /* DMA1 Ch1 = DMAMUX ch 0 */

    DMA1_Channel1->CCR   = 0;
    DMA1_Channel1->CPAR  = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR  = (uint32_t)buf;
    DMA1_Channel1->CNDTR = len;
    DMA1_Channel1->CCR   = DMA_CCR_MINC | DMA_CCR_CIRC
                         | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0   /* 16-bit */
                         | DMA_CCR_PL_0;                       /* medium prio */
    DMA1_Channel1->CCR  |= DMA_CCR_EN;

    /* 8. Enable ADC, wait for ready, start converting */
    ADC1->ISR = ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)) { }
    ADC1->CR |= ADC_CR_ADSTART;
}

