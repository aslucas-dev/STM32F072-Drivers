/**
  ***************************************************************************************
  * @file    systick.c
  * @author  Lucas
  * @date    Mar 21, 2026
  ***************************************************************************************
  */


//.......................................................................................
#include "systick.h"
#include "stm32f0xx.h"
//.......................................................................................
static volatile uint32_t systick_count = 0;
//.......................................................................................
#define SYSTICK_MAX_RELOAD 0x00FFFFFFUL
//.......................................................................................

/**====================================================================================*
 *  @brief  Interrupt Service Routine for the SysTick Timer.                           *
 *          Increments the global tick counter on each overflow.                       *
 *                                                                                     *
 *  @param  None                                                                       *
 *                                                                                     *
 *  @retval None                                                                       *
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    systick_count++;
}

/**====================================================================================*
 *  @brief  Disables the SysTick timer and clears its configuration registers.         *
 *          Ref. PM0215 Section 4.4 (SysTick timer registers).                         *
 *                                                                                     *
 *  @param  None                                                                       *
 *                                                                                     *
 *  @retval None                                                                       *
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
static void reset(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
}

/**====================================================================================*
 *  @brief  Configures the SysTick reload value and enables the counter/interrupt.     *
 *          Ref. PM0215 Section 4.4.1 (STK_CSR) and 4.4.2 (STK_RVR).                   *
 *                                                                                     *
 *  @param  frequency_tick: Desired tick frequency (Hz).                               *
 *  @param  systick_clock:  Current system clock (Hz) used by the core.                *
 *                                                                                     *
 *  @retval None                                                                       *
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
static void config(uint32_t frequency_tick, uint32_t systick_clock)
{
    SysTick->LOAD = (systick_clock / frequency_tick) - 1;
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick->CTRL =  SysTick_CTRL_CLKSOURCE_Msk |
            SysTick_CTRL_TICKINT_Msk |
            SysTick_CTRL_ENABLE_Msk;
}

 /**===================================================================================*
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
 void systick_init(uint32_t frequency_tick, uint32_t systick_clock)
 {
    reset();

    if ((frequency_tick == 0) || 
        (systick_clock == 0) ||
        (frequency_tick > systick_clock) ||
        ((systick_clock / frequency_tick) > (SYSTICK_MAX_RELOAD + 1))) {
        return;
    }
    
    config(frequency_tick, systick_clock);
 }

 /**===================================================================================*
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
 void systick_deinit(void)
 {
    reset();
 }

 /**===================================================================================*
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/ 
 uint32_t systick_get_tick(void)
 {
     uint32_t retval = 0;

     retval = systick_count;

     return retval;
 }

 /**===================================================================================*
 *                                                                                     *
 *-------------------------------------------------------------------------------------*/
 void systick_delay(uint32_t ticks_delay)
 {
     uint32_t time_mark = systick_get_tick();

     while(( systick_get_tick() - time_mark) < ticks_delay){
        __NOP();
    }
 }
