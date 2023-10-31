/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l1xx_hal.h"
#include "stm32l1xx_ll_adc.h"
#include "stm32l1xx_ll_dac.h"
#include "stm32l1xx_ll_dma.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_exti.h"
#include "stm32l1xx_ll_cortex.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_usart.h"
#include "stm32l1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AKT_Pin GPIO_PIN_13
#define AKT_GPIO_Port GPIOC
#define RS6_Pin GPIO_PIN_14
#define RS6_GPIO_Port GPIOC
#define RM6_Pin GPIO_PIN_15
#define RM6_GPIO_Port GPIOC
#define RS5_Pin GPIO_PIN_0
#define RS5_GPIO_Port GPIOH
#define RM5_Pin GPIO_PIN_1
#define RM5_GPIO_Port GPIOH
#define RM1_Pin GPIO_PIN_1
#define RM1_GPIO_Port GPIOB
#define RS1_Pin GPIO_PIN_2
#define RS1_GPIO_Port GPIOB
#define RM2_Pin GPIO_PIN_10
#define RM2_GPIO_Port GPIOB
#define RS2_Pin GPIO_PIN_11
#define RS2_GPIO_Port GPIOB
#define RM3_Pin GPIO_PIN_12
#define RM3_GPIO_Port GPIOB
#define RS3_Pin GPIO_PIN_13
#define RS3_GPIO_Port GPIOB
#define RM4_Pin GPIO_PIN_14
#define RM4_GPIO_Port GPIOB
#define RS4_Pin GPIO_PIN_15
#define RS4_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define E6_Pin GPIO_PIN_9
#define E6_GPIO_Port GPIOA
#define E4_Pin GPIO_PIN_10
#define E4_GPIO_Port GPIOA
#define E5_Pin GPIO_PIN_11
#define E5_GPIO_Port GPIOA
#define DE_Pin GPIO_PIN_12
#define DE_GPIO_Port GPIOA
#define E2_Pin GPIO_PIN_15
#define E2_GPIO_Port GPIOA
#define E3_Pin GPIO_PIN_3
#define E3_GPIO_Port GPIOB
#define TST_Pin GPIO_PIN_4
#define TST_GPIO_Port GPIOB
#define E1_Pin GPIO_PIN_5
#define E1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */