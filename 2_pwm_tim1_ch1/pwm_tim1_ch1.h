/**
  ***************************************************************************************
  * @file    pwm_tim1_ch1.h
  * @author  Lucas
  * @date    Apr 26, 2026
  * @brief   PWM driver for TIM1 Channel 1 (including complementary channel).
  *       
  @verbatim  
  =======================================================================================
                      #####  Specific Features #####
  =======================================================================================
    [..]  
      (+) Configuration of TIM1 Channel 1 (PA8) and/or Channel 1N (PA7).
      (+) Support for inverted phase (polarity) on the channels.
      (+) Direct register manipulation (CMSIS) for maximum efficiency.

  =======================================================================================
                      #####  Limitations #####
  =======================================================================================
    [..]  
      (-) Fixed pins on PA8 (CH1) and PA7 (CH1N) for this specific driver.
      (-) Resolution limited by the peripheral clock and 16-bit prescaler.
      (-) Supported PWM frequency up to 16kHz when peripheral clock is 48MHz.

  @endverbatim
  ***************************************************************************************
  */

#ifndef PWM_TIM1_CH1_H_
#define PWM_TIM1_CH1_H_

//.......................................................................................
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
//.......................................................................................

/* Priority definitions for driver initialization */
#define PWM_TIM1_CH1_PRIORITY_HIGHEST     0
#define PWM_TIM1_CH1_PRIORITY_HIGH        1
#define PWM_TIM1_CH1_PRIORITY_LOW         2
#define PWM_TIM1_CH1_PRIORITY_LOWEST      3

//.......................................................................................

typedef enum {
    PWM_TIM1_CH1_MODE_DISABLED = 0,
    PWM_TIM1_CH1_MODE_CH1,
    PWM_TIM1_CH1_MODE_CH1N,
    PWM_TIM1_CH1_MODE_CH1_CH1N
} pwm_tim1_ch1_mode_e;

//.......................................................................................

/**
 * @brief  Initializes the PWM TIM1 CH1 peripheral with the specified parameters.
 *         Configures GPIOs PA8 (CH1) and PA7 (CH1N), Timer 1, and interrupts.
 * 
 * @param  mode: Operation mode (CH1, CH1N, or both).
 * @param  inverted_phase: Defines whether the channel phase is inverted.
 * @param  prphl_clock: Peripheral clock frequency feeding the TIM1.
 * 
 * @retval bool: true if initialization was successful, false otherwise.
 **/
bool pwm_tim1_ch1_init(pwm_tim1_ch1_mode_e mode, bool inverted_phase, uint32_t prphl_clock);

//.......................................................................................

/**
 * @brief  Deinitializes the PWM TIM1 CH1 peripheral and associated GPIOs.
 *         Restores registers to their reset state.
 * 
 * @param  None
 * 
 * @retval None
 **/
void pwm_tim1_ch1_deinit(void);

//.......................................................................................

#endif /* PWM_TIM1_CH1_H_ */


