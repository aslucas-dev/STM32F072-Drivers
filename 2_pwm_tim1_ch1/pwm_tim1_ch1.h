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
                      #####  Resolution Calculation #####
  =======================================================================================
    [..]  
      The driver uses dynamic prescaling to maximize resolution. 
      The number of discrete PWM steps (for duty cycle adjustment) is:

      Steps = Peripheral_Clock / (Target_Frequency * (PSC + 1))

      Where:
      PSC = floor(Peripheral_Clock / (Target_Frequency * 65536))

      Example (assuming 48MHz clock):
      (+) At 1 kHz:  PSC = 0, Steps = 48,000 (~15.5 bits)
      (+) At 16 kHz: PSC = 0, Steps = 3,000  (~11.5 bits)
      (+) At 48 kHz: PSC = 0, Steps = 1,000  (0.1% resolution)
      (+) At 1 Hz:   PSC = 732, Steps = 65,484 (~16 bits)

  =======================================================================================
                      #####  Limitations #####
  =======================================================================================
    [..]  
      (-) Fixed pins on PA8 (CH1) and PA7 (CH1N) for this specific driver.
      (-) Resolution decreases as the target frequency increases.
      (-) Max frequency is capped at 48kHz by the API to maintain at
          least 0.1% resolution.
      (-) Frequency range: ~0.7 Hz to 48 kHz.

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

/**
 * @brief  Sets the PWM frequency.
 *         Calculates the optimal prescaler (PSC) and reload value (ARR) to 
 *         maximize resolution for the given frequency.
 * 
 * @param  frequency: Target frequency in Hz.
 * 
 * @retval None
 **/
void pwm_set_frequency(uint32_t frequency);

//.......................................................................................

/**
 * @brief  Sets the PWM duty cycle.
 *         Supports fine adjustment from 0.0% to 100.0%.
 * 
 * @param  duty_cycle: Value from 0 to 1000 (representing 0.0% to 100.0%).
 * 
 * @retval None
 **/
void pwm_set_dutycycle(uint16_t duty_cycle);

//.......................................................................................

#endif /* PWM_TIM1_CH1_H_ */


