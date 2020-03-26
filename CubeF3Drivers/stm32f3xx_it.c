/************************************************************************************//**
 * @file stm32f3xx_it.c
 * @brief   Interrupt Service Routines.
 *
 * @author  Teru
 * @date    2019/06/19
 * @version Rev0.10
 * @par �ύX����:
 * - Rev0.10: 2019/06/19: �V�K�쐬.
 *
 * @copyright 2019 Emb-se.com.
 */
/**
 * @addtogroup GROUP_Interrupt �����݃n���h��.
 * @{
 */
#include "stm32f3xx_hal.h"

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
	while(1);
}

/**
 * @brief This function handles System service call via SWI instruction.
 * FreeRTOS�ōĒ�`����
 */
__weak void SVC_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 * FreeRTOS�ōĒ�`����
 */
__weak void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 * FreeRTOS�ōĒ�`����
 */
__weak void SysTick_Handler(void)
{
    HAL_IncTick();
}
