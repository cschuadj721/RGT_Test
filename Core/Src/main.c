/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "memorymap.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "lcd.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* Enable TRC */


#ifdef __GNUC__
/* GCC: __io_putchar() is called by printf() */
int __io_putchar(int ch)
#else
/* Keil: fputc() is called by printf() */
int fputc(int ch, FILE *f)
#endif
{
    /* Wait until ITM port 0 is ready */
    while ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0) { }
    /* Send character */
    ITM_SendChar(ch);
    return ch;
}



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
	
	/* Configure the MPU attributes for the QSPI 256MB without instruction access */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Configure the MPU attributes for the QSPI 8MB (QSPI Flash Size) to Cacheable WT */
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress      = QSPI_BASE;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_8MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

static void LED_Blink(uint32_t Hdelay,uint32_t Ldelay)
{
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_SET);
	HAL_Delay(Hdelay - 1);
	HAL_GPIO_WritePin(E3_GPIO_Port,E3_Pin,GPIO_PIN_RESET);
	HAL_Delay(Ldelay-1);
}

/**
  * @brief  Get the current time and date.
  * @param  
  * @retval None
  */
static void RTC_CalendarShow(RTC_DateTypeDef *sdatestructureget,RTC_TimeTypeDef *stimestructureget)
{
  /* ����ͬʱ��ȡʱ������� ��Ȼ�ᵼ���´�RTC���ܶ�ȡ */
  /* Both time and date must be obtained or RTC cannot be read next time */
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, sdatestructureget, RTC_FORMAT_BIN);
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
#ifdef W25Qxx
  SCB->VTOR = QSPI_BASE;
#endif
  MPU_Config();
  CPU_CACHE_Enable();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_SPI4_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  LCD_Test();
  //LCD 3rd Line fill-in blank
    LCD_ShowString(4, 40, ST7735Ctx.Width, 16, 16, "                                         ");
    char msg[32];

    //SPI Flash Init
    if (W25Qx_Init() != W25Qx_OK) {
        LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, (uint8_t*)"Init Fail");
        Error_Handler();
    }

    //SPI Flash ID check
    uint16_t id;
    W25Qx_Read_ID(&id);
    sprintf(msg, "ID: %04X", id);
    LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, (uint8_t*)msg);
    HAL_Delay(1000);


    //SPI Flash Erase
    if (W25Qx_Erase_Block(0) != W25Qx_OK) {
        LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, (uint8_t*)"Erase Fail");
        Error_Handler();
    }

    //SPI Flash Write
    uint8_t writeBuffer[] = "Hello Flash!";
    if (W25Qx_Write(writeBuffer, 0, sizeof(writeBuffer)) != W25Qx_OK) {
        LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, (uint8_t*)"Write Fail");
        Error_Handler();
    }

    //SPI Flash Read
    uint8_t readBuffer[32] = {0};
    if (W25Qx_Read(readBuffer, 0, sizeof(writeBuffer)) != W25Qx_OK) {
        LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, (uint8_t*)"Read Fail");
        Error_Handler();
    }

    // Display read data from SPI Flash on 4th Row of LCD
    LCD_ShowString(0, 58, ST7735Ctx.Width, 16, 16, readBuffer);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	  int16_t raw = (int16_t)HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);

	  /* 2) full-scale ±Vref = ±3.3V maps to ±32768 */
	   const float Vref = 3.3f;
	   float voltage = ((float)raw) * Vref / 32768.0f;

	   /* 3) round to nearest millivolt */
	   int32_t mV = (int32_t)(voltage * 1000.0f + (raw >= 0 ? 0.5f : -0.5f));


	   /* 4) split sign, integer and frac */
	   bool    neg    = (mV < 0);
	   int32_t abs_mV = neg ? -mV : mV;
	   int32_t v_int  = abs_mV / 1000;
	   int32_t v_frac = abs_mV % 1000;

	   /* 5) one-shot print with at most one ‘–’ */
	   char buf[32];
	   snprintf(buf, sizeof(buf),  " %ld.%03ld V", v_int, v_frac);

	   /* 6) show it on the LCD */
	   LCD_ShowString(4, 40, ST7735Ctx.Width, 16, 16, (uint8_t*)buf);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 12;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
typedef struct {
    float voltage_V;    // battery voltage in volts
    float current_mA;   // battery current in milliamps
} BatteryStatus_t;

BatteryStatus_t Read_BatteryVoltageCurrent(void)
{
    BatteryStatus_t st;
    const float Vref = 3.3f;   // ADC reference = Vdd

    // --- start conversion of both ranks ---
    HAL_ADC_Start(&hadc1);

    // 1) Battery voltage (rank 1)
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    int16_t rawV = HAL_ADC_GetValue(&hadc1);

    // 2) Shunt voltage (rank 2)
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    int16_t rawI = HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);

    // differential full-scale = ±Vref
    st.voltage_V  = ((float)rawV / 32768.0f) * Vref;
    float v_shunt = ((float)rawI / 32768.0f) * Vref;

    // compute current = Vshunt / Rshunt  (Rshunt in ohms)
    const float Rshunt = 0.1f;
    st.current_mA = (v_shunt / Rshunt) * 1000.0f;

    return st;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  LED_Blink(30,30);
  LED_Blink(30,30);
  LED_Blink(30,30);


  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
