/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define ANALOG_OUT_BLOCK_A						0x01
#define ANALOG_OUT_BLOCK_B						0x02

#define ANALOG_OUT_BUFFER_SIZE					2048

typedef enum {
	Mode_None,
	Mode_DC,
	Mode_Sine,
	Mode_Square,
	Mode_Ramp,
	Mode_Noise
} AnalogOutMode;

typedef struct {
	AnalogOutMode mode;
	uint16_t offset;
	uint32_t frequency;
	uint16_t amplitude;
	uint8_t dtc;
} AnalogOutCHConfigStruct;

typedef struct {
	uint8_t channel;
	uint32_t frequency;
	uint16_t buffer[ANALOG_OUT_BUFFER_SIZE];
	uint16_t bufferLength;
} AnalogOutCHStruct;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */


#if defined( __ICCARM__ )
  #define DMA_BUFFER \
      _Pragma("location=\".dma_buffer\"")
#else
  #define DMA_BUFFER \
      __attribute__((section(".dma_buffer")))
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void AnalogOutInit();
void AnalogOutConfigChannel(uint8_t anBlock, uint8_t channel, AnalogOutCHStruct config);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
