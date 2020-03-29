/**
 * @file  TIMER_ms.h
 * @brief ミリ秒タイマコントローラの制御API.
 * 
 * ミリ秒タイマコントローラの制御APIを提供する.
 * 
 * @author      Teru
 * @date        2019/08/04
 * @version     Rev0.10
 * 
 * @par 変更履歴:
 * - Rev0.01: 2019/06/27: 新規作成
 * - Rev0.10: 2019/08/04: 機能名（タイマ→ミリ秒タイマ）変更
 * 
 * @copyright   2019 Emb-se.com.
 */
#ifndef _TIMER_MS_H_
#define _TIMER_MS_H_

#include "stm32_assert.h"
#include "stm32f3xx_ll_tim.h"

/**
 * @page page_timer_ms タイマコントローラ(ミリ秒)制御.
 *
 * @section sec1_timer_ms タイマコントローラについて.
 * - タイマコントローラは、TIM4-ch1を使用する.
 * - タイマコントローラのクロックソース(APB1 Timer clock)は、内部クロック 48MHzである.
 * - 初期設定にて、T=1/(48MHz/9600)=0.0002=0.2msecとする.
 *
 * @section sec2_api_timer_ms タイマコントローラ制御API.
 * | APIs              | 機能名称    |
 * |:------------------ |:---------- |
 * | TIMER_ms_init()       | タイマコントローラの初期設定 |
 * | TIMER_ms_setCompReg() | コンペアマッチ値セット |
 * | TIMER_ms_getTick()    | tickカウンタ取得 |
 * | TIM4_IRQHandler()     | コンペアマッチ割込みハンドラ |
 *
 * @section sec3_usage_timer_ms 使用方法.
 * - 予め一度だけ TIMER_init() を呼び出す.
 */
/**
 * @addtogroup group_timer_ms タイマコントローラ(ミリ秒)制御.
 * @brief タイマコントローラ制御APIを提供する.<br>
 * @{
 */

/// ミリ秒タイマコントローラ
#define TIMER_MS_DEV					TIM4
/// ミリ秒タイマコントローラIRQn
#define TIMER_MS_IRQ					TIM4_IRQn

/// tickレジスタ型(16bitカウンタ)
typedef uint16_t    TICK16_t;
/// タイマカウンタ型(パワーオンからの経過tick)
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
 * @brief タイマコントローラの初期設定.
 */
extern void TIMER_ms_init( void );

/**
 * @brief コンペアマッチ値セット.
 */
#define TIMER_ms_setCompReg( val )		LL_TIM_OC_SetCompareCH1( TIMER_MS_DEV, val )

/**
 * @brief tickレジスタ値の取得.
 */
#define TIMER_ms_getTick16()				LL_TIM_GetCounter( TIMER_MS_DEV )
/**
 * @brief タイマカウンタ(32,64bit)値の取得.
 */
extern uint32_t  TIMER_ms_getTick32(void);
extern uint64_t  TIMER_ms_getTick64(void);

/**
 * @brief TIM4 割込みハンドラ.
 */
extern void TIM4_IRQHandler(void);

/**
 * @} end of addtogroup
 */
#endif //_TIMER_MS_H_
