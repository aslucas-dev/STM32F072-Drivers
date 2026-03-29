/**
  ***************************************************************************************
  * @file    usart1.h
  * @author  Lucas
  * @date    Mar 29, 2026
  *       
  @verbatim  
  =======================================================================================
                      #####  Specific Features #####
  =======================================================================================
    [..]  
      (+) Core-to-metal direct register manipulation (pure CMSIS).
      (+) 128-byte software-controlled circular buffers (FIFO) for RX and TX.
      (+) Interrupt-driven asynchronous operation (RXNE and TXE).
      (+) Thread-safe public API using NVIC protection during buffer access.
      (+) Automatic GPIO configuration (PA9/PA10) in Alternate Function mode.

  =======================================================================================
                      #####  Limitations #####
  =======================================================================================
    [..]  
      (-) Fixed buffer size (128 bytes), defined at compile time.
      (-) Supports only standard 8N1 frame (8 bits, No parity, 1 stop bit).
      (-) Hardware Flow Control (RTS/CTS) is not implemented.
      (-) Depends on accurate peripheral clock (PCLK) provided by the user.

  @endverbatim
  ***************************************************************************************
  */

#ifndef USART1_USART1_H_
#define USART1_USART1_H_

//.......................................................................................
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
//.......................................................................................
/* Priority definitions for driver initialization */
#define USART1_PRIORITY_HIGHEST     0
#define USART1_PRIORITY_HIGH        1
#define USART1_PRIORITY_LOW         2
#define USART1_PRIORITY_LOWEST      3
//.......................................................................................
/**
 * @brief  Initializes the USART1 peripheral with the specified parameters.
 *         Configures GPIOs (PA9/TX, PA10/RX), Baudrate, and Interrupts.
 * 
 * @param  isrPriority: NVIC priority for USART1 interrupts. 
 * @param  prphlClock:  Peripheral clock frequency (Hz) feeding the USART.
 * @param  baudrate:    Target communication speed (bits/s).
 * 
 * @retval bool: true if initialization was successful, false otherwise.
 **/
bool usart1_init(uint32_t isrPriority, uint32_t prphlClock, uint32_t baudrate);

/**
 * @brief  Reads a single byte from the internal RX circular buffer.
 *         This operation is non-blocking and thread-safe.
 * 
 * @param  data: Pointer to the variable where the data will be stored.
 * 
 * @retval bool: true if a byte was successfully read, false if the buffer was empty.
 **/
bool usart1_read(uint8_t *data);

/**
 * @brief  Transmits a single byte by adding it to the internal TX buffer.
 *         The transmission is handled via interrupts (TXE).
 * 
 * @param  data: Byte to be sent.
 * 
 * @retval bool: true if data was enqueued for transmission, false if the buffer is full.
 **/
bool usart1_write(uint8_t data);

//.......................................................................................
#endif /* USART1_USART1_H_ */
