/**
  ***************************************************************************************
  * @file    systick.h
  * @author  Lucas
  * @date    Mar 21, 2026
  * @brief   Driver for the ARM Cortex-M0 SysTick Timer.
  *          Provides a centralized system tick counter and basic delay functions.
  *          
  *          References:
  *          - STM32F0xxxx Cortex-M0 Programming Manual (PM0215), Section 4.4.
  *       
  @verbatim                
  =======================================================================================
                      #####  Specific Features #####
  =======================================================================================
    [..]  
      (+) 24-bit down-counter with auto-reload capability.
      (+) Core-integrated peripheral, providing consistent timing across Cortex-M devices.
      (+) Generates a recurring interrupt for system-wide timekeeping (systick_count).
      (+) Atomicity: On Cortex-M0 (32-bit), aligned 32-bit word access is a single-cycle atomic operation (LDR).
          This makes systick_get_tick() thread-safe without disabling interrupts.

                      #####  Limitations #####
  =======================================================================================
    [..]  
      (+) Hardware Limit (24-bit): The STK_RVR (Reload Value Register) is limited to 
          0xFFFFFF (16.777.215). This limits the maximum period between ticks.
          If (systick_clock / frequency_tick) exceeds this, the timer cannot be 
          initialized with the requested frequency.

      (+) CPU Starvation: Using very high tick frequencies (e.g., 1MHz / 1us ticks) 
          is NOT recommended. Each tick triggers an ISR; at 1MHz, the CPU spend
          most of its time entering and exiting the interrupt handler, starving 
          the main application and other low-priority tasks of processing power.
          - Recommended: Use SysTick for millisecond ticks (1kHz).
          - For Microseconds: Use a dedicated hardware timer (TIMx) for better efficiency.

      (+) Precision: Accuracy is directly tied to the AHB clock stability.

  @endverbatim
  ***************************************************************************************
  * @attention
  *
  ***************************************************************************************
  */

#ifndef SYSTICK_SYSTICK_H_
#define SYSTICK_SYSTICK_H_
//.......................................................................................
#include "stdbool.h"
#include "stdint.h"
//.......................................................................................

 /**
 *  @brief  Initializes the SysTick system timer with the specified frequency.
 *          Note: The reload value (clock/freq) must not exceed 24 bits (0xFFFFFF).
 *
 *  @param  frequency_tick: Number of ticks per second (e.g., 1000 for 1ms ticks).
 *  @param  systick_clock:  Frequency of the clock driving SysTick (usually AHB).
 *
 *  @retval None
 *
 **/
 void systick_init(uint32_t frequency_tick, uint32_t systick_clock);

 /** 
 *  @brief  Stops the SysTick timer and releases the resource.
 * 
 *  @param  None
 * 
 *  @retval None
 * 
 **/ 
 void systick_deinit(void);

 /**
  *  @brief  Returns the current system tick count since initialization.
  * 
  *  @param  None
  * 
  *  @retval uint32_t: Current tick value.
  * 
  **/
 uint32_t systick_get_tick(void);

 /** 
 *  @brief  Creates a blocking delay using the system tick counter.
 * 
 *  @param  ticks_delay: Number of ticks to wait (based on init frequency).
 * 
 *  @retval None
 * 
 **/
 void systick_delay(uint32_t ticks_delay);
//.......................................................................................
#endif /* 0_SYSTICK_SYSTICK_H_ */
