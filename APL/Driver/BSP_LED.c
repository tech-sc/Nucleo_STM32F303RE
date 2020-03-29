/************************************************************************************//**
 * @file BSP_LED.c
 * @brief BluePill�I���{�[�hLED����API
 *
 * @author  Teru
 * @date    2018/12/24
 * @version Rev0.10
 * @par �ύX����:
 * - Rev0.10: 2018/12/24: �V�K�쐬
 *
 * @copyright 2018 Emb-se.com. All rights reserved.
 */
/**
 * @addtogroup GROUP_BSP_LED
 * @{
 */
#include "BSP_LED.h"

/**
 * @brief �I���{�[�hLED�̏����ݒ�
 *
 * LED�̓v���_�E���ݒ��Ainactive(1)�ŏ���������B
 * @param �Ȃ�
 * @return �Ȃ�
 */
void BSP_LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

	/** @li LED�����ݒ� */
	BSP_LED_turnOFF();

	/* Configure GPIO pin */
	GPIO_InitStruct.Pin   = LED_PIN;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init( LED_POPT, &GPIO_InitStruct );
}

/**
 * @} end of addtogroup
 */
