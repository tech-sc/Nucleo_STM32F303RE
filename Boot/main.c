/************************************************************************************//**
 * @file    main.c
 * @brief   STM32F303RE STM32Nucleo
 *
 * @mainpage STM32Nucleo STM32F303RE �����v���W�F�N�g
 * @brief   �g���݃V�X�e���Z�p��p�v���W�F�N�g�ł���.<br>
 *
 * @details STM32CubeF3���C�u�������g�p���Ď�������.<br>
 *
 * @par �ύX����:
 *	Rev0.01�@2019/06/02�@�e���v���[�g�쐬<br>
 * @par Copyright
 *	2014-19 Emb-se.com All rights reserved.
 */
#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_cortex.h"
#include "OS.h"

/**
 * @brief �I���{�[�h�y���t�F���������ݒ�
 * @retval None
 */
__weak void BSP_init(void)
{
}

/**
 * @brief �A�v�������ݒ�
 * @retval None
 */
__weak void APL_init(void)
{
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    while(1);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {
        Error_Handler();
    }
    LL_RCC_HSI_Enable();
    /* Wait till HSI is ready */
    while(LL_RCC_HSI_IsReady() != 1);

    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_8, LL_RCC_PREDIV_DIV_1);
    LL_RCC_PLL_Enable();
    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1);

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) ;

    //LL_Init1msTick(64000000);
    SystemCoreClock = configCPU_CLOCK_HZ;
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(SystemCoreClock);
}

/**
 * @brief   �v���O�������C��
 * @param   None
 * @retval  None
 */
void main( void )
{
    /* Configure the system clock */
    SystemClock_Config();
    HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );

    BSP_init();

    APL_init();

	osKernel_start();
    for(;;);
}
