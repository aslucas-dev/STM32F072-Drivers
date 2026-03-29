/**
  ***************************************************************************************
  * @file    usart1_conf.h
  * @author  Lucas
  * @date    Mar 29, 2026
  * @brief   Configuration header for USART1 driver.
  *          This file contains software limits and buffer size definitions.
  ***************************************************************************************
  */

#ifndef USART1_USART1_CONF_H_
#define USART1_USART1_CONF_H_

//.......................................................................................

/**
 * @brief  Size of the software RX circular buffer (in bytes).
 * @note   Increasing this value consumes more RAM.
 **/
#define USART1_RX_QUEUE_SIZE 128

/**
 * @brief  Size of the software TX circular buffer (in bytes).
 * @note   Increasing this value consumes more RAM.
 **/
#define USART1_TX_QUEUE_SIZE 128

//.......................................................................................
#endif /* USART1_USART1_CONF_H_ */
