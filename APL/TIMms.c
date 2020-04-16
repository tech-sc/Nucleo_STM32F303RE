/**
 * @file  TIMms.c
 * @brief ミリ秒オーダータイマ機能.
 * 
 * ミリ秒単位で要求可能なソフトウェアタイマをAPLに提供する.
 * 
 * @author		Teru
 * @date		2019/12/21
 * @version		Rev0.10
 * 
 * @par 変更履歴:
 * - Rev0.01: 2019/06/27: 新規作成.
 * - Rev0.02: 2019/07/27: Doxygen対応.
 * - Rev0.10: 2019/12/21: TIMER_msドライバの変更に対応.
 * 
 * @copyright	2019 Emb-se.com.
 */
/**
 * @addtogroup GROUP_TIMms ミリ秒オーダータイマ機能.
 * @{
 */
#include "TIMER_ms.h"
#include "os.h"
#include "TIMms.h"
//#include "ExtLED.h"		///計測用.

/// タイマ要求リストポインタ
static TIMms_t		*TIMms_TopReq;
/// タイマ要求リストポインタ
static TIMms_t		*TIMms_TailReq;
/// タイマ発火リストポインタ
static TIMms_t		*TIMms_TopExpire;

/// 次回発火tick値
static TICK16_t		TIMms_NextExpire;

/// タイマ発火タスクを起床するための同期化変数
static osMutexHandle_t	TIMms_ExpireMutex;

#if 0 //LowLevelのTIMERに移行した.
/// パワーオンからの経過tick
static union {
	struct {
		TICK16_t	lo;
		TICK16_t	hi;
	}tick_w;
	uint32_t	tick_l;
} TIMms_Tick;
#endif

/**
 * @brief ミリ秒タイマ発火処理タスク.
 * @param[in]	arg	 未使用.
 */
static void TIMms_task( void *arg )
{
	TIMms_t		**exp, *p;

	while( 1 ){
		osMutex_take( TIMms_ExpireMutex, osMAX_TIME );

		if( TIMms_TopExpire != NULL ){
			exp = &TIMms_TopExpire;
			while( *exp != NULL ) {
				p = *exp;
				*exp = p->next_list;
				(p->expire_cb)( p );
			}
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

	TIMms_NextExpire = tick;
}

/**
 * @brief ミリ秒タイマ機能の初期設定API
 *
 * ミリ秒タイマコントローラの初期設定及びタイマ発火タスクの生成
 */
void TIMms_initTimer( void )
{
	osTaskHandle_t	handle;
	int		retv;

printf("%s\n", __FUNCTION__);
	/* タイマ起動 */
	TIMER_ms_init();

	/* 内部管理データ初期化 */
	//TIMms_Tick.tick_l = 0;
	TIMms_TopReq	 = NULL;
	TIMms_TailReq	 = NULL;
	TIMms_TopExpire	 = NULL;
	TIMms_NextExpire = (TICK16_t)-1;	//最大値セット

	/* OSリソース生成 */
	TIMms_ExpireMutex = osMutex_create();

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
	TICK16_t  tmp_tick;

	configASSERT(time > 0);
	configASSERT(expire_cb != NULL);
	configASSERT(p_req != NULL);

	p_req->old_tick	   = TIMER_ms_getTick32();
	p_req->remain_tick = MSEC_TO_TICK(time);
	p_req->expire_cb   = expire_cb;
	p_req->next_list   = NULL;
	p_req->feature	   = TIMms_FEATURE;

	osEnterCritical();
	if( TIMms_TailReq != NULL ){
		TIMms_TailReq->next_list = p_req;
		TIMms_TailReq = p_req;
	}else{
		TIMms_TailReq = p_req;
		TIMms_TopReq  = p_req;
	}
	osExitCritical();

	tmp_tick  = p_req->old_tick;
	tmp_tick += p_req->remain_tick;
	if( tmp_tick < TIMms_NextExpire ){
		TIMms_setNextExpire( tmp_tick );
	}
	return p_req;
}


/**
 * @brief ミリ秒タイマ取消API
 *
 * タイマ要求を取消すAPI関数である.
 * APIの使い方によっては取消と発火がすれ違う可能性があるので呼び出し側で対応すること.
 *
 * @param[in]	handle	タイマ要求を取消すタイマハンドル.
 * @retval	!NULL	取消が成功すると、取消したタイマハンドルを返す.
 * @retval	NULL	パラメータエラー.
 *					またはエントリされてない（既に発火した）.
 * @pre		引数のhandleはNULLでないこと.
 */
void *TIMms_cancelTimer( TIMms_t *handle )
{
	TIMms_t		**pre, *p;

	osEnterCritical();
	pre = &TIMms_TopReq;
	p = TIMms_TopReq;
	while(p != NULL) {
		if( p == handle ) {
			if( TIMms_TailReq == handle ) {
				TIMms_TailReq = (*pre)->next_list;
			}
			*pre = p->next_list;
			break;
		}
		pre = &p->next_list;
		p = p->next_list;
	}
	osExitCritical();
	return p;
}

/**
 * @brief ミリ秒タイマ割込みハンドラからコールされる発火処理.
 * @param[in]	over =0/=1:カウンタOverflow.
 */
void TIMER_ms_expire( int over )
{
	portBASE_TYPE	dispatch;
	TIMms_t		**pre, *p;
	TIMms_t		**exp;
	TICK16_t	min_tick;
	uint32_t	temp_tick, now_tick;

	if( over ){
		// 計測用
		//ExtLED1_toggle();
	}

	dispatch  = pdFALSE;
	min_tick = (TICK16_t)-1;	//set MAX
	now_tick = TIMER_ms_getTick32();

	osEnterCritical();
	exp = &TIMms_TopExpire;
	while( *exp != NULL ) {
		*exp = (*exp)->next_list;
	}
	pre = &TIMms_TopReq;
	p = TIMms_TopReq;
	while(p != NULL) {
        temp_tick = now_tick - p->old_tick;

		p->remain_tick -= temp_tick;
		if( p->remain_tick <= 0 ) {
			 *pre = p->next_list;
			 p->next_list = NULL;
			 *exp = p;
			 exp = &p->next_list;
			 p = *pre;
		} else {
			if( p->remain_tick < min_tick ) {
				min_tick = (TICK16_t)p->remain_tick;
			}
			p->old_tick = now_tick;
			p = p->next_list;
		}
	}
	if( TIMms_TopReq == NULL ) {
		TIMms_TailReq = NULL;
	}
	TIMms_setNextExpire( min_tick );
	osExitCritical();

	if( TIMms_TopExpire != NULL ){
		/* タスクを起床する */
		osMutex_giveISR( TIMms_ExpireMutex, &dispatch );
		portEND_SWITCHING_ISR( dispatch );
	}
}

/**
 * @} end of addtogroup
 */
