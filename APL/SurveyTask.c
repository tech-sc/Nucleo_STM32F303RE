/**************************************************************************//**
 * @file	SurveyTask.c
 * @brief	タスク優先度調査
 * 
 * @version V0.10.0
 * @date	2020/04/13
 * @author	Teru
 * 
 * @par 変更履歴:
 * - Rev0.10: 2020/04/13: 新規作成.
 * 
 * @copyright	2019-20 Emb-se.com.
*/
#include "os.h"
#include "TIMms.h"
#include "UART.h"

enum {
	TASK_A = 0,
	TASK_B,
	TASK_NUM
};
/// タイマ発火待ちハンドラ
static osMutex_h		expire_mutex[TASK_NUM];
static	TIMms_t			tmr_handle[TASK_NUM];

/**************************************************************************/
static void tmr_expireA( void *p_handle )
{
	osMutex_give( expire_mutex[TASK_A] );
}
static void tmr_expireB( void *p_handle )
{
	osMutex_give( expire_mutex[TASK_B] );
}

/**************************************************************************//**
 * @brief	タスク優先度調査タスク
 * @param[in]	arg		タスク起動パラメータ(='A'/'B'/'C')
 */
void SurveyTask( void *arg )
{
	uint32_t		wakeup_tim, now_tim;

	switch( *(char*)arg ){
	case 'A':
		expire_mutex[TASK_A] = osMutex_create();
		break;
	case 'B':
		expire_mutex[TASK_B] = osMutex_create();
		break;
	default:
		return;
	}

	while( 1 ){
		switch( *(char*)arg ){
		case 'A':
			TIMms_reqTimer( 50, &tmr_expireA, &tmr_handle[TASK_A] );
			osMutex_take( expire_mutex[TASK_A], osBLOCKING );
			break;
		case 'B':
			TIMms_reqTimer( 50, &tmr_expireB, &tmr_handle[TASK_B] );
			osMutex_take( expire_mutex[TASK_B], osBLOCKING );
			break;
		default:
			return;
		}

		wakeup_tim = TICK_TO_MSEC( TIMms_getTick() );
		do {
			UART_putchar( *(char*)arg );
			now_tim = TICK_TO_MSEC( TIMms_getTick() );
		}while( (now_tim - wakeup_tim) < 20 );
	}
}

/**************************************************************************//**
 * @brief	タスクを生成する
 */
void SurveyTask_init( void )
{
	osError		retv;
	osTask_h	handle;

	retv = osTask_create( &SurveyTask, "SurveyTaskA", SurveyA_STACKSZ/4,
						"A", SurveyA_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);

	retv = osTask_create( &SurveyTask, "SurveyTaskB", SurveyB_STACKSZ/4,
						"B", SurveyB_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);
}
