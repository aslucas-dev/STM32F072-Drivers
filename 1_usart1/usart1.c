/**
  ***************************************************************************************
  * @file    usart1.c
  * @author  Lucas
  * @date    Mar 29, 2026
  ***************************************************************************************
  */

//.......................................................................................
#include "usart1.h"
#include "stm32f0xx.h"
//.......................................................................................

/**====================================================================================*
 * @brief  Applies a full reset to USART1 peripheral and its associated GPIOs (PA9/10).*
 *         Reference: Ref. RM0091 Section 27.8 (USART registers) and 8.4.12 (APB2RSTR).*
 *                                                                                     * 
 * @param  None                                                                        * 
 * @retval None                                                                        * 
 *-------------------------------------------------------------------------------------*/
static void apply_reset(void)
{
    NVIC_DisableIRQ(USART1_IRQn);

    RCC->APB2ENR &= ~((uint32_t)RCC_APB2ENR_USART1EN);
    RCC->CFGR3 &= ~((uint32_t)RCC_CFGR3_USART1SW);
    RCC->CFGR3 |= (uint32_t)RCC_CFGR3_USART1SW_PCLK;
    RCC->APB2RSTR |= (uint32_t)RCC_APB2RSTR_USART1RST;
    RCC->APB2ENR |= (uint32_t)RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= (uint32_t)RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~((uint32_t)GPIO_MODER_MODER10 | GPIO_MODER_MODER9);
    GPIOA->OTYPER &= ~((uint32_t)GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_9);
    GPIOA->OSPEEDR &= ~((uint32_t)GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR9);
    GPIOA->PUPDR &= ~((uint32_t)GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR9);
    GPIOA->AFR[1] &= ~((uint32_t)GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL9);
}

/**====================================================================================*
 * @brief  Removes the reset state from USART1 peripheral.                             *
 *         Reference: Ref. RM0091 Section 8.4.12 (APB2RSTR).                           *
 *                                                                                     * 
 * @param  None                                                                        * 
 * @retval None                                                                        * 
 *-------------------------------------------------------------------------------------*/
static void remove_reset(void)
{
    RCC->APB2RSTR &= ~((uint32_t)RCC_APB2RSTR_USART1RST);
}

/**====================================================================================*
 * @brief  Configures USART1 with specified baudrate and clock.                        *
 *         Reference: Ref. RM0091 Section 27.5.3 (Baud rate generation).               *
 *                                                                                     * 
 * @param  isrPriority: NVIC interrupt priority.                                       * 
 * @param  prphlClock:  Peripheral clock frequency (Hz).                               * 
 * @param  baudrate:    Target baud rate (bits/s).                                     * 
 * @retval None                                                                        * 
 *-------------------------------------------------------------------------------------*/
static void configure(uint32_t isrPriority, uint32_t prphlClock, uint32_t baudrate)
{
    GPIOA->MODER |= ((uint32_t)GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1);
    GPIOA->OTYPER |= (uint32_t)GPIO_OTYPER_OT_10;
    GPIOA->PUPDR |= (uint32_t)GPIO_PUPDR_PUPDR10_0;
    GPIOA->AFR[1] |=
            (uint32_t)((0x01 << GPIO_AFRH_AFSEL10_Pos) | (0x01 << GPIO_AFRH_AFSEL9_Pos));

    USART1->BRR = ((uint32_t)prphlClock / baudrate);

    //
    USART1->CR1 |= ((uint32_t)USART_CR1_RE | // RX enable
            USART_CR1_TE |           // TX enable
            USART_CR1_UE |           // UART enable
            USART_CR1_RXNEIE);       // Receive data interrupt

    NVIC_SetPriority(USART1_IRQn, isrPriority);
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);
}
