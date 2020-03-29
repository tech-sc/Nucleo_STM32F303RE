/**
 * @file  TIMER_ms.c
 * @brief �~���b�^�C�}�R���g���[���̐���API.
 * 
 * �~���b�^�C�}�R���g���[���̐���API��񋟂���.
 * 
 * @author      Teru
 * @date        2019/08/04
 * @version     Rev0.10
 * 
 * @par �ύX����:
 * - Rev0.01: 2019/06/27: �V�K�쐬
 * - Rev0.10: 2019/08/04: �@�\���i�^�C�}���~���b�^�C�}�j�ύX
 * 
 * @copyright   2019 Emb-se.com.
 */
/**
 * @addtogroup GROUP_TIMER_MS
 * @{
 */
#include "TIMER_ms.h"

#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_bus.h"

/// �^�C�}�J�E���^64bit
static TICK64_t TIMER_cnt;

/**
 * @brief �~���b�^�C�}�R���g���[���̏����ݒ�.
 */
void TIMER_ms_init( void )
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};
	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_TIM4 );
	__HAL_DBGMCU_FREEZE_TIM4();

	/* TIMER_MS_DEV interrupt Init */
	NVIC_SetPriority( TIMER_MS_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0) );
	NVIC_EnableIRQ( TIMER_MS_IRQ );

	/* setup */
	TIM_InitStruct.Prescaler = 9600;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 0xffff;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init( TIMER_MS_DEV, &TIM_InitStruct );
	LL_TIM_DisableARRPreload( TIMER_MS_DEV );
	LL_TIM_SetClockSource( TIMER_MS_DEV, LL_TIM_CLOCKSOURCE_INTERNAL );
	TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_ACTIVE;
	TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.CompareValue = 0;
	TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_LOW;
	LL_TIM_OC_Init( TIMER_MS_DEV, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct );
	LL_TIM_OC_DisableFast( TIMER_MS_DEV, LL_TIM_CHANNEL_CH1 );
	LL_TIM_SetTriggerOutput( TIMER_MS_DEV, LL_TIM_TRGO_CC1IF );
	LL_TIM_DisableMasterSlaveMode( TIMER_MS_DEV );

	TIMER_cnt.tick64 = 0;
	LL_TIM_ClearFlag_CC1( TIMER_MS_DEV );
	LL_TIM_ClearFlag_UPDATE( TIMER_MS_DEV );
	/* �J�E���^�X�^�[�g */
	LL_TIM_EnableIT_CC1( TIMER_MS_DEV );
    LL_TIM_EnableIT_UPDATE( TIMER_MS_DEV );
	LL_TIM_EnableCounter( TIMER_MS_DEV );
}

/**
 * @brief 32�r�b�gTick�l�̎擾.
 * @retval	32�r�b�gTick�l.
 */
uint32_t  TIMER_ms_getTick32(void)
{
	TIMER_cnt.tick32w.tick16w.lo = TIMER_ms_getTick16();
	return TIMER_cnt.tick32w.tick32;
}

/**
 * @brief 64�r�b�gTick�l�̎擾.
 * @retval	64�r�b�gTick�l.
 */
uint64_t  TIMER_ms_getTick64(void)
{
	TIMER_cnt.tick32w.tick16w.lo = TIMER_ms_getTick16();
	return TIMER_cnt.tick64;
}

/**
 * @brief �~���b�^�C�}���Ώ���.
 * @param[in]	over =0/=1:�J�E���^Overflow.
 */
__weak void TIMER_ms_expire( int over )
{
}


/**
 * @brief TIMER_MS_DEV �����݃n���h��.
 *
 * �~���b�^�C�}�@�\�̔��Ώ������R�[������.
 */
void TIM4_IRQHandler(void)
{
	int		over;
    TIMER_cnt.tick32w.tick16w.lo = TIMER_ms_getTick16();
	/* �R���y�A�}�b�` */
	if( LL_TIM_IsActiveFlag_CC1( TIMER_MS_DEV ) ) {
		LL_TIM_ClearFlag_CC1( TIMER_MS_DEV );
		over = 0;
	}
	/* �J�E���^�I�[�o�[�t���[ */
	if( LL_TIM_IsActiveFlag_UPDATE( TIMER_MS_DEV ) ) {
		LL_TIM_ClearFlag_UPDATE( TIMER_MS_DEV );
		if( TIMER_cnt.tick32w.tick16w.hi == 0xFFFF ) {
			TIMER_cnt.tick32w.tick16w.hi = 0;
			TIMER_cnt.tick32w.tick32 ++;
		}else{
			TIMER_cnt.tick32w.tick16w.hi ++;
		}
		over = 1;
	}
	/** @li �^�C�}�@�\�̔��Ώ��� */
	TIMER_ms_expire( over );
}

/**
 * @} end of addtogroup
 */
