/**
 * @file  TIMER_ms.h
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
#ifndef _TIMER_MS_H_
#define _TIMER_MS_H_

#include "stm32_assert.h"
#include "stm32f3xx_ll_tim.h"

/**
 * @page page_timer_ms �^�C�}�R���g���[��(�~���b)����.
 *
 * @section sec1_timer_ms �^�C�}�R���g���[���ɂ���.
 * - �^�C�}�R���g���[���́ATIM4-ch1���g�p����.
 * - �^�C�}�R���g���[���̃N���b�N�\�[�X(APB1 Timer clock)�́A�����N���b�N 48MHz�ł���.
 * - �����ݒ�ɂāAT=1/(48MHz/9600)=0.0002=0.2msec�Ƃ���.
 *
 * @section sec2_api_timer_ms �^�C�}�R���g���[������API.
 * | APIs              | �@�\����    |
 * |:------------------ |:---------- |
 * | TIMER_ms_init()       | �^�C�}�R���g���[���̏����ݒ� |
 * | TIMER_ms_setCompReg() | �R���y�A�}�b�`�l�Z�b�g |
 * | TIMER_ms_getTick()    | tick�J�E���^�擾 |
 * | TIM4_IRQHandler()     | �R���y�A�}�b�`�����݃n���h�� |
 *
 * @section sec3_usage_timer_ms �g�p���@.
 * - �\�߈�x���� TIMER_init() ���Ăяo��.
 */
/**
 * @addtogroup group_timer_ms �^�C�}�R���g���[��(�~���b)����.
 * @brief �^�C�}�R���g���[������API��񋟂���.<br>
 * @{
 */

/// �~���b�^�C�}�R���g���[��
#define TIMER_MS_DEV					TIM4
/// �~���b�^�C�}�R���g���[��IRQn
#define TIMER_MS_IRQ					TIM4_IRQn

/// tick���W�X�^�^(16bit�J�E���^)
typedef uint16_t    TICK16_t;
/// �^�C�}�J�E���^�^(�p���[�I������̌o��tick)
typedef  union {
	struct {
		TICK16_t	lo;
		TICK16_t	hi;
	}tick16w;
	uint32_t	tick32;
} TICK32_t;
typedef union {
	TICK32_t  tick32w;
	uint64_t  tick64;
}TICK64_t;

/**
 * @brief �^�C�}�R���g���[���̏����ݒ�.
 */
extern void TIMER_ms_init( void );

/**
 * @brief �R���y�A�}�b�`�l�Z�b�g.
 */
#define TIMER_ms_setCompReg( val )		LL_TIM_OC_SetCompareCH1( TIMER_MS_DEV, val )

/**
 * @brief tick���W�X�^�l�̎擾.
 */
#define TIMER_ms_getTick16()				LL_TIM_GetCounter( TIMER_MS_DEV )
/**
 * @brief �^�C�}�J�E���^(32,64bit)�l�̎擾.
 */
extern uint32_t  TIMER_ms_getTick32(void);
extern uint64_t  TIMER_ms_getTick64(void);

/**
 * @brief TIM4 �����݃n���h��.
 */
extern void TIM4_IRQHandler(void);

/**
 * @} end of addtogroup
 */
#endif //_TIMER_MS_H_
