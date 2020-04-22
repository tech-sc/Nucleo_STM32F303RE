/**
 * @file  TIMms.c
 * @brief ミリ秒オーダータイマ機能.
 * 
 * ミリ秒単位で要求可能なソフトウェアタイマをAPLに提供する.
 * 
 * @author		Teru
 * @date		2020/04/22
 * @version		Rev0.21
 * 
 * @par 変更履歴:
 * - Rev0.01: 2019/06/27: 新規作成.
 * - Rev0.02: 2019/07/27: Doxygen対応.
 * - Rev0.10: 2019/12/21: TIMER_msドライバの変更に対応.
 * - Rev0.20: 2020/04/21: 双方向リスト管理に変更.
 * - Rev0.21: 2020/04/22: 発火tick値を16bit→64bitに改良.
 * 
 * @copyright   2019-20 Emb-se.com.
 */
/**
 * @addtogroup GROUP_TIMms ミリ秒オーダータイマ機能.
 * @{
 */
#include "os.h"
#include "ListEx.h"
#include "TIMER_ms.h"
#include "TIMms.h"
//#include "ExtLED.h"		///計測用.


/// タイマ要求リストポインタ
static ListHdr_t	TIMms_Request;

/// タイマ発火リストポインタ
static ListHdr_t	TIMms_Expire;

/// 次回発火tick値
static TICK64_t		TIMms_NextExpire;

/// タイマ発火タスクを起床するための同期化変数
static osMutex_h	TIMms_ExpireMutex;

/**
 * @brief ミリ秒タイマ発火処理タスク.
 * @param[in]	arg	 未使用.
 */
static void TIMms_task( void *arg )
{
	TIMms_t		*p_exp, *p_temp;

	/* OSリソース生成 */
	TIMms_ExpireMutex = osMutex_create();

	while( 1 ){
		osMutex_take( TIMms_ExpireMutex, osBLOCKING );

		p_exp = (TIMms_t*)List_getNext( &TIMms_Expire );
		while( p_exp != NULL ) {
			List_remove( &TIMms_Expire, &p_exp->list );
			p_temp = (TIMms_t*)List_getNext( &p_exp->list );
			(p_exp->expire_cb)( p_exp );
			p_exp = p_temp;
		}
	}
}

/**
 * @brief 次回発火TICK値セット.
 * @param[in] tick	次発火TICK値.
 */
static void TIMms_setNextExpire( TICK16_t tick )
{
	/* コンペアマッチレジスタを変更する. */
	TIMER_ms_setCompReg( tick );
}

/**
 * @brief ミリ秒タイマ機能の初期設定API
 *
 * ミリ秒タイマコントローラの初期設定及びタイマ発火タスクの生成
 */
void TIMms_initTimer( void )
{
	osTask_h	handle;
	osError		retv;

	/* タイマ起動 */
	TIMER_ms_init();

	/* 内部管理データ初期化 */
	List_init( &TIMms_Request );
	List_init( &TIMms_Expire );
	TIMms_NextExpire.tick64 = (uint64_t)-1;	//最大値セット

	/* 発火コールバック用タスクを生成する. */
	retv = osTask_create( &TIMms_task, "TIMms", TIMms_STACKSZ/4,
						NULL, TIMmsTASK_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT( retv == pdPASS );
}

/**
 * @brief ミリ秒タイマ要求API
 *
 * ミリ秒単位のタイマ要求を提供するAPI関数である.
 * 発火はコールバックにて通知する.
 *
 * @param[in]	time	   タイマ時間。ミリ秒で指定する.
 * @param[in]	expire_cb  発火時のコールバック関数を指定する.
 * @param[out]	p_req	   タイマ要求データ領域を渡す.メンバ変数には何も設定する必要はない.
 *						   但し発火する迄、データ領域を保持する事.
 * @retval	handle	タイマ要求が成功すると、そのタイマハンドルを返す.
 * @retval	NULL	パラメータエラー.
 * @pre		引数 timeは1ms以上であること.
 *			引数 expire_cbはNULLでないこと.
 *			引数 p_reqはNULLでないこと.
 * @post	タイマ取消は返したハンドルで停止可能である. また発火時に該当ハンドルをパラメータで通知する.
 */
void *TIMms_reqTimer( int32_t time, void (*expire_cb)(void *handle), TIMms_t *p_req )
{
	TICK64_t  tmp_tick;

	configASSERT(time > 0);
	configASSERT(expire_cb != NULL);
	configASSERT(p_req != NULL);

	p_req->old_tick	   = TIMER_ms_getTick32();
	p_req->remain_tick = MSEC_TO_TICK(time);
	p_req->expire_cb   = expire_cb;
	p_req->feature	   = TIMms_FEATURE;

	osEnterCritical();
	if( List_find( &TIMms_Request, &p_req->list ) != NULL ) {
		osExitCritical();
		return NULL;
	}
	List_add( &TIMms_Request, &p_req->list );
	osExitCritical();

	tmp_tick.tick64 = TIMER_ms_getTick64();
	tmp_tick.tick64 += p_req->remain_tick;
	if( tmp_tick.tick64 <= TIMms_NextExpire.tick64 ){
		TIMms_NextExpire.tick64 = tmp_tick.tick64;
		TIMms_setNextExpire( TIMms_NextExpire.tick32w.tick16w.lo );
	}
	return p_req;
}


/**
 * @brief ミリ秒タイマ取消API
 *
 * タイマ要求を取消すAPI関数である.
 * APIの使い方によっては取消と発火がすれ違う可能性があるので呼び出し側で対応すること.
 *
 * @param[in]	p_req	タイマ要求を取消すタイマハンドル.
 * @retval	!NULL	取消が成功すると、取消したタイマハンドルを返す.
 * @retval	NULL	パラメータエラー.
 *					またはエントリされてない（既に発火した）.
 * @pre		引数のhandleはNULLでないこと.
 */
void *TIMms_cancelTimer( TIMms_t *p_req )
{
	configASSERT(p_req != NULL);

	osEnterCritical();
	List_remove( &TIMms_Request, &p_req->list );
	osExitCritical();
	return p_req;
}

/**
 * @brief ミリ秒タイマ割込みハンドラからコールされる発火処理.
 * @param[in]	over =0/=1:カウンタOverflow.
 */
void TIMER_ms_expire( int over )
{
	portBASE_TYPE	dispatch;
	TIMms_t		*p_req, *p_temp;
	uint32_t	min_tick;
	uint32_t	temp_tick, now_tick;

	if( over ){
		// 計測用
		//ExtLED1_toggle();
	}

	dispatch  = pdFALSE;
	min_tick = (uint32_t)-1;	//set MAX
	now_tick = TIMER_ms_getTick32();

	osEnterCritical();
	p_req = (TIMms_t*)List_getNext( &TIMms_Request );
	while( p_req != NULL ) {
		temp_tick = now_tick - p_req->old_tick;
		p_req->remain_tick -= temp_tick;

		if( p_req->remain_tick <= 0 ) {
			List_remove( &TIMms_Request, &p_req->list );
			p_temp = (TIMms_t*)List_getNext( &p_req->list );
			List_add( &TIMms_Expire, &p_req->list );
			p_req = p_temp;
		} else {
			if( p_req->remain_tick < min_tick ) {
				min_tick = p_req->remain_tick;
			}
			p_req = (TIMms_t*)List_getNext( &p_req->list );
		}
	}
	osExitCritical();

	TIMms_NextExpire.tick64 = TIMER_ms_getTick64();
	if( min_tick == (uint32_t)-1 ) {
		TIMms_NextExpire.tick32w.tick16w.lo = (TICK16_t)-1;
	} else {
		TIMms_NextExpire.tick64 += min_tick;
	}
	TIMms_setNextExpire( TIMms_NextExpire.tick32w.tick16w.lo );

	p_temp = (TIMms_t*)List_getNext( &TIMms_Expire );
	if( p_temp != NULL ){
		/* タイマ発火処理タスクを起床する */
		osMutex_giveISR( TIMms_ExpireMutex, &dispatch );
		portEND_SWITCHING_ISR( dispatch );
	}
}

/**
 * @} end of addtogroup
 */
