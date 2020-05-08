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

typedef enum {
	TASK_A = 0,
	TASK_B,
	TASK_NUM
} TASK_e;

#define REPEAT_NUM		96			// =38400bps/(1+8+1)/1000ms*25ms
static TASK_e			ExpireSW;
static TIMms_t			TimerHandle;
static osMutex_h		ExpireMutex[TASK_NUM];
static const uint8_t	OutChar[TASK_NUM] = { 'A', 'B' };

/**************************************************************************/
static void Survey_TimerExpire( void *p_handle )
{
	if( ExpireSW == TASK_A ) {
		ExpireSW = TASK_B;
	} else {
		ExpireSW = TASK_A;
	}
	TIMms_reqTimer( 20, &Survey_TimerExpire, &TimerHandle );
	osMutex_give( ExpireMutex[ExpireSW] );
}

/**************************************************************************//**
 * @brief	タスク優先度調査タスク
 * @param[in]	arg		タスク起動パラメータ(='A'/'B')
 */
void SurveyTask( void *arg )
{
	int			i;
	TASK_e		task_prm;
	uint8_t		out_char = *(uint8_t*)arg;

	switch( out_char ) {
	case 'A':
		task_prm = TASK_A;
		break;
	case 'B':
		task_prm = TASK_B;
		break;
	default:
		return;
	}
	ExpireMutex[task_prm] = osMutex_create();

	while( 1 ){
		osMutex_take( ExpireMutex[task_prm], osBLOCKING );

		for( i=0; i < 96; i++ ) {
			UART_putchar( out_char );
		}
	}
}

/**************************************************************************//**
 * @brief	タスクを生成する
 */
void SurveyTask_init( void )
{
	osError		retv;
	osTask_h	handle;

	retv = osTask_create( &SurveyTask, "SurveyTaskA", SurveyA_STACKSZ/4, (void*)&OutChar[TASK_A],
						   SurveyA_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);

	retv = osTask_create( &SurveyTask, "SurveyTaskB", SurveyB_STACKSZ/4, (void*)&OutChar[TASK_B],
						  SurveyB_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);

	ExpireSW = TASK_A;
	TIMms_reqTimer( 20, &Survey_TimerExpire, &TimerHandle );
}
