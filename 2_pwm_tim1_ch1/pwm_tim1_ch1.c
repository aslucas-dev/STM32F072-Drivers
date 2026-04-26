/**
  ***************************************************************************************
  * @file    pwm_tim1_ch1.c
  * @author  Lucas
  * @date    Apr 26, 2026
  ***************************************************************************************
  */

//.......................................................................................
#include "pwm_tim1_ch1.h"
#include "stm32f0xx.h"
//.......................................................................................
#define PWM_FREQUENCY_BASE      16000000
//.......................................................................................
static pwm_tim1_ch1_mode_e pwm_ch_mode = PWM_TIM1_CH1_MODE_DISABLED;

/**====================================================================================*
  * @brief  Applies reset signal to TIM1 peripheral and clears GPIO configurations.    *
  *         Reference: Ref. RM0091 Section 6.4                                         *
  *         Reference: Ref. RM0091 Section 8.4  (GPIO Registers)                       *
  *                                                                                    *
  * @param  mode: Operation mode to identify which GPIOs to reset.                     *
  * @retval None                                                                       *
  *------------------------------------------------------------------------------------*/
static void apply_reset(pwm_tim1_ch1_mode_e mode)
{
    RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    switch (mode) {
    //...................................................................................
        case PWM_TIM1_CH1_MODE_CH1:
            GPIOA->MODER &= ~((uint32_t)GPIO_MODER_MODER8);
            GPIOA->OTYPER &= ~((uint32_t)GPIO_OTYPER_OT_8);
            GPIOA->OSPEEDR &= ~((uint32_t)GPIO_OSPEEDER_OSPEEDR8);
            GPIOA->PUPDR &= ~((uint32_t)GPIO_PUPDR_PUPDR8);
            GPIOA->AFR[1] &= ~((uint32_t)GPIO_AFRH_AFSEL8);
        break;
        //...............................................................................
        case PWM_TIM1_CH1_MODE_CH1N:
            GPIOA->MODER &= ~((uint32_t)GPIO_MODER_MODER7);
            GPIOA->OTYPER &= ~((uint32_t)GPIO_OTYPER_OT_7);
            GPIOA->OSPEEDR &= ~((uint32_t)GPIO_OSPEEDER_OSPEEDR7);
            GPIOA->PUPDR &= ~((uint32_t)GPIO_PUPDR_PUPDR7);
            GPIOA->AFR[0] &= ~((uint32_t)GPIO_AFRL_AFSEL7);
        break;
        //...............................................................................
        case PWM_TIM1_CH1_MODE_CH1_CH1N:
            /* Reset PA8 for CH1 */
            GPIOA->MODER &= ~((uint32_t)GPIO_MODER_MODER8);
            GPIOA->OTYPER &= ~((uint32_t)GPIO_OTYPER_OT_8);
            GPIOA->OSPEEDR &= ~((uint32_t)GPIO_OSPEEDER_OSPEEDR8);
            GPIOA->PUPDR &= ~((uint32_t)GPIO_PUPDR_PUPDR8);
            GPIOA->AFR[1] &= ~((uint32_t)GPIO_AFRH_AFSEL8);
            /* Reset PA7 for CH1N */
            GPIOA->MODER &= ~((uint32_t)GPIO_MODER_MODER7);
            GPIOA->OTYPER &= ~((uint32_t)GPIO_OTYPER_OT_7);
            GPIOA->OSPEEDR &= ~((uint32_t)GPIO_OSPEEDER_OSPEEDR7);
            GPIOA->PUPDR &= ~((uint32_t)GPIO_PUPDR_PUPDR7);
            GPIOA->AFR[0] &= ~((uint32_t)GPIO_AFRL_AFSEL7);
        break;
    //...................................................................................
    }
}

/**====================================================================================*
  * @brief  Removes reset signal from TIM1 peripheral.                                 *
  *         Reference: Ref. RM0091 Section 6.4                                         *
  *                                                                                    *
  * @param  None                                                                       *
  * @retval None                                                                       *
  *------------------------------------------------------------------------------------*/
static void remove_reset(void)
{
    RCC->APB2RSTR &= ~((uint32_t)RCC_APB2RSTR_TIM1RST);
}

/**====================================================================================*
  * @brief  Configures TIM1 and GPIO registers for PWM operation.                      *
  *         This function defines GPIO and timer operating modes for PWM generation.   *
  *         The prescaler is configured to support frequencies up to 16kHz             *
  *         assuming a peripheral clock of 48MHz.                                      *
  *         Reference: Ref. RM0091 Section 17.4 (TIM1 Registers)                       *
  *         Reference: Ref. RM0091 Section 8.4  (GPIO Registers)                       *
  *                                                                                    *
  * @param  mode: PWM operation mode.                                                  *
  * @param  inverted_phase: Defines polarity for the output channels.                  *
  * @param  prphl_clock: Bus clock frequency for prescaler calculation.                *
  * @retval None                                                                       *
  *------------------------------------------------------------------------------------*/
static void configure(pwm_tim1_ch1_mode_e mode, bool inverted_phase, uint32_t prphl_clock)
{
    switch (mode) {
    //...................................................................................
        case PWM_TIM1_CH1_MODE_CH1:
            /* Configure PA8 for CH1 */
            GPIOA->MODER |= GPIO_MODER_MODER8_1;
            GPIOA->AFR[1] |= ((uint32_t)(0x02 << GPIO_AFRH_AFSEL8_Pos));

            TIM1->CCER |= TIM_CCER_CC1E;
            if (inverted_phase) {
                TIM1->CCER |= TIM_CCER_CC1P;
            }
        break;
        //...............................................................................
        case PWM_TIM1_CH1_MODE_CH1N:
            /* Configure PA7 for CH1N */
            GPIOA->MODER |= GPIO_MODER_MODER7_1;
            GPIOA->AFR[0] |= ((uint32_t)(0x02 << GPIO_AFRL_AFSEL7_Pos));

            TIM1->CCER |= TIM_CCER_CC1NE;
            if (inverted_phase) {
                TIM1->CCER |= TIM_CCER_CC1NP;
            }
        break;
        //...............................................................................
        case PWM_TIM1_CH1_MODE_CH1_CH1N:
            /* Configure PA8 for CH1 and PA7 for CH1N */
            GPIOA->MODER |= GPIO_MODER_MODER8_1;
            GPIOA->AFR[1] |= ((uint32_t)(0x02 << GPIO_AFRH_AFSEL8_Pos));

            GPIOA->MODER |= GPIO_MODER_MODER7_1;
            GPIOA->AFR[0] |= ((uint32_t)(0x02 << GPIO_AFRL_AFSEL7_Pos));

            TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE);
            if (inverted_phase) {
                TIM1->CCER |= (TIM_CCER_CC1P | TIM_CCER_CC1NP);
            }
        break;
    //...................................................................................
    }

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->PSC = ((prphl_clock / PWM_FREQUENCY_BASE) - 1);
    TIM1->ARR = 0;
    TIM1->CCR1 = 0;

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;

    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->EGR |= TIM_EGR_UG;
    TIM1->CR1 |= TIM_CR1_CEN;
}

/**====================================================================================*
  * @brief  See public documentation in pwm_tim1_ch1.h                                 *
  *------------------------------------------------------------------------------------*/
bool pwm_tim1_ch1_init(pwm_tim1_ch1_mode_e mode,
        bool inverted_phase,
        uint32_t prphl_clock)
{
    if((mode > PWM_TIM1_CH1_MODE_CH1_CH1N) || (mode < PWM_TIM1_CH1_MODE_CH1)){
        return false;
    }

    pwm_ch_mode = mode;

    apply_reset(mode);
    remove_reset();
    configure(mode, inverted_phase, prphl_clock);

    return true;
}

/**====================================================================================*
  * @brief  See public documentation in pwm_tim1_ch1.h                                 *
  *------------------------------------------------------------------------------------*/
void pwm_tim1_ch1_deinit(void)
{
    if((pwm_ch_mode > PWM_TIM1_CH1_MODE_CH1_CH1N) ||
            (pwm_ch_mode < PWM_TIM1_CH1_MODE_CH1)){
        return ;
    }

    apply_reset(pwm_ch_mode);
    remove_reset();
}
