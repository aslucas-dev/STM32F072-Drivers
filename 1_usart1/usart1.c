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
#define USART1_RX_QUEUE_SIZE 128
#define USART1_TX_QUEUE_SIZE 128
//.......................................................................................

/* Macro for dynamic priority validation based on CPU hardware bits */
#define IS_NVIC_PRIORITY_VALID(PRIO) ((PRIO) < (1 << __NVIC_PRIO_BITS))
//.......................................................................................
typedef struct {
    uint32_t isr_status;
    uint8_t data;
    uint8_t rx_queue[USART1_RX_QUEUE_SIZE];
    uint8_t tx_queue[USART1_TX_QUEUE_SIZE];
    uint8_t rx_head;
    uint8_t rx_tail;
    uint8_t tx_head;
    uint8_t tx_tail;
} driver_usart_context_t;
//.......................................................................................
static driver_usart_context_t drvr_usart_cntxt = {0};
//.......................................................................................

/**====================================================================================*
 * @brief  Adds a new byte to the RX circular buffer.                                  *
 *                                                                                     * 
 * @param  data: Byte received from the peripheral.                                    * 
 * @retval bool: true if successful, false if buffer is full.                          * 
 *-------------------------------------------------------------------------------------*/
static bool enqueue_rx_data(uint8_t data)
{
    if (((drvr_usart_cntxt.rx_head + 1) % USART1_RX_QUEUE_SIZE) ==
            drvr_usart_cntxt.rx_tail) {
        return false;
    }

    drvr_usart_cntxt.rx_queue[drvr_usart_cntxt.rx_head] = data;
    drvr_usart_cntxt.rx_head = (drvr_usart_cntxt.rx_head + 1) % USART1_RX_QUEUE_SIZE;

    return true;
}

/**====================================================================================*
 * @brief  Retrieves a byte from the RX circular buffer.                               *
 *                                                                                     * 
 * @param  data: Pointer to store the retrieved byte.                                  * 
 * @retval bool: true if data was available, false if buffer is empty.                 * 
 *-------------------------------------------------------------------------------------*/
static bool dequeue_rx_data(uint8_t *data)
{
    if (drvr_usart_cntxt.rx_head == drvr_usart_cntxt.rx_tail) {
        return false;
    }

    *data = drvr_usart_cntxt.rx_queue[drvr_usart_cntxt.rx_tail];
    drvr_usart_cntxt.rx_tail = (drvr_usart_cntxt.rx_tail + 1) % USART1_RX_QUEUE_SIZE;
    return true;
}

/**====================================================================================*
 * @brief  Adds a byte to the TX circular buffer for later transmission.               *
 *                                                                                     * 
 * @param  data: Byte to be transmitted.                                               * 
 * @retval bool: true if successful, false if buffer is full.                          * 
 *-------------------------------------------------------------------------------------*/
static bool enqueue_tx_data(uint8_t data)
{
    if (((drvr_usart_cntxt.tx_head + 1) % USART1_TX_QUEUE_SIZE) ==
            drvr_usart_cntxt.tx_tail) {
        return false;
    }

    drvr_usart_cntxt.tx_queue[drvr_usart_cntxt.tx_head] = data;
    drvr_usart_cntxt.tx_head = (drvr_usart_cntxt.tx_head + 1) % USART1_TX_QUEUE_SIZE;

    return true;
}

/**====================================================================================*
 * @brief  Retrieves a byte from the TX circular buffer for transmission.              *
 *                                                                                     * 
 * @param  data: Pointer to store the byte to be sent.                                 * 
 * @retval bool: true if data was available, false if buffer is empty.                 * 
 *-------------------------------------------------------------------------------------*/
static bool dequeue_tx_data(uint8_t *data)
{
    if (drvr_usart_cntxt.tx_head == drvr_usart_cntxt.tx_tail) {
        return false;
    }

    *data = drvr_usart_cntxt.tx_queue[drvr_usart_cntxt.tx_tail];
    drvr_usart_cntxt.tx_tail = (drvr_usart_cntxt.tx_tail + 1) % USART1_TX_QUEUE_SIZE;

    return true;
}

/**====================================================================================*
 * @brief  USART1 Interrupt Service Routine. Handles RXNE, ORE, and TXE events.        *
 *         Reference: Ref. RM0091 Section 27.8 (USART registers).                      *
 *                                                                                     * 
 * @param  None                                                                        * 
 * @retval None                                                                        * 
 *-------------------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
    drvr_usart_cntxt.isr_status = USART1->ISR;
    NVIC_ClearPendingIRQ(USART1_IRQn);

    if (drvr_usart_cntxt.isr_status & USART_ISR_RXNE) {
        USART1->ICR |= (uint32_t)USART_ICR_ORECF;

        drvr_usart_cntxt.data = USART1->RDR;

        enqueue_rx_data(drvr_usart_cntxt.data);
    }

    if (drvr_usart_cntxt.isr_status & USART_ISR_ORE) {
        USART1->ICR |= (uint32_t)USART_ICR_ORECF;
    }

    if (drvr_usart_cntxt.isr_status & USART_ISR_TXE) {
        USART1->ICR |= (uint32_t)USART_ICR_TCCF;

        if (dequeue_tx_data(&drvr_usart_cntxt.data)) {
            USART1->TDR = drvr_usart_cntxt.data;
        } else {
            USART1->CR1 &= ~((uint32_t)USART_CR1_TXEIE);
        }
    }
}

/**====================================================================================*
 * @brief  Applies a full reset to USART1 peripheral and its associated GPIOs (PA9/10).
 *         Reference: Ref. RM0091 Section 27.8 (USART registers) and 8.4.12 (APB2RSTR).
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

/**====================================================================================*
 * @brief  See public documentation in usart1.h                                        *
 *-------------------------------------------------------------------------------------*/
bool usart1_init(uint32_t isrPriority, uint32_t prphlClock, uint32_t baudrate)
{
    if ((baudrate == 0) || (prphlClock == 0) || !IS_NVIC_PRIORITY_VALID(isrPriority)) {
        return false;
    }

    apply_reset();
    remove_reset();
    configure(isrPriority, prphlClock, baudrate);

    return true;
}

/**====================================================================================*
 * @brief  See public documentation in usart1.h                                        *
 *-------------------------------------------------------------------------------------*/
bool usart1_read(uint8_t *data)
{
    bool ret_val = false;

    if (data == NULL) {
        return false;
    }

    NVIC_DisableIRQ(USART1_IRQn);

    if (dequeue_rx_data(data)) {
        ret_val = true;
    } else {
        ret_val = false;
    }

    NVIC_EnableIRQ(USART1_IRQn);

    return ret_val;
}

/**====================================================================================*
 * @brief  See public documentation in usart1.h                                        *
 *-------------------------------------------------------------------------------------*/
bool usart1_write(uint8_t data)
{
    bool ret_val = false;

    NVIC_DisableIRQ(USART1_IRQn);

    if (enqueue_tx_data(data)) {
        USART1->CR1 |= USART_CR1_TXEIE;
        ret_val = true;
    } else {
        ret_val = false;
    }

    NVIC_EnableIRQ(USART1_IRQn);

    return ret_val;
}
