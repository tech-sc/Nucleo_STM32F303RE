/**
 * @file  TIMms.h
 * @brief ミリ秒オーダータイマ機能.
 * 
 * ミリ秒単位で要求可能なソフトウェアタイマをAPLに提供する.
 * 
 * @author      Teru
 * @date		2020/04/21
 * @version		Rev0.20
 * 
 * @par 変更履歴:
 * - Rev0.01: 2019/06/27: 新規作成.
 * - Rev0.02: 2019/07/27: Doxygen対応.
 * - Rev0.10: 2019/12/21: TIMER_msドライバの変更に対応.
 * - Rev0.20: 2020/04/21: 双方向リスト管理に変更.
 * 
 * @copyright   2019-20 Emb-se.com.
 */
#ifndef _TIMms_H_
#define _TIMms_H_

#include "ListEx.h"
#include "TIMER_ms.h"

/**
 * @page TIMms_title タイマライブラリ
 *
 * @section TIMms_api API一覧
 * | API名          | 機能          |
 * |:-------------- |:------------- |
 * | TIMms_initTimer()  | 初期設定   |
 * | TIMms_reqTimer()   | タイマ要求 |
 * | TIMms_cancelTimer()| タイマ取消 |
 *
 * @section TIMms_usage 使用方法
 * - 予め一度だけ TIMms_initTimer() を呼び出す。
 */
/**
 * @addtogroup GROUP_TIMms ミリ秒オーダータイマ機能.
 * @{
 */

/// タイマカウンタレジスタ型(16bitカウンタ)
// typedef uint16_t    TIMCNT_t;

/**
 * @brief タイマ要求データ型.
 */
typedef struct TIMms_t {
	/// 要求または発火リスト
	ListHdr_t	list;
    /// 発火までの残りTICK
    int64_t     remain_tick;
    /// 発火コールバック関数.
    void   (*expire_cb)(void *hdl);
//    /// 要求または発火リスト.
//    struct TIMms_t *next_list;
    /// 要求処理または発火処理時のTICK値.
    uint32_t    old_tick;
    /// オプション機能コード.
    uint16_t    feature;
}TIMms_t;

/**
 * @brief オプション機能コード.
 */
enum TIMms_OPTCD {
	TIMms_FEATURE	= 0x5000,		/**< ミリ秒オーダータイマ機能コード */
	TIMms_STARTING	= 0x0100,		/**< タイマ要求処理中 */
	TIMms_STOPING	= 0x0200,		/**< タイマ停止処理中 */
	TIMms_ONESHORT	= 0x0010,		/**< OneShot */
	TIMms_REPEAT	= 0x0020,		/**< リピート */
};

/// ミリ秒→tick変換. 1TICK=0.20msec
#define MSEC_TO_TICK(ms)		(int64_t)((int64_t)ms * 5) +1
/// tick→ミリ秒変換. 1TICK=0.20msec
#define TICK_TO_MSEC(tick)		(int32_t)(tick / 5)

#define TIMms_getTick()     TIMER_ms_getTick32()

/**
 * @brief 初期設定API
 */
extern void TIMms_initTimer( void );

/**
 * @brief タイマ要求API
 */
extern void *TIMms_reqTimer( int32_t time, void (*expire_cb)(void *handle), TIMms_t *p_req );

/**
 * @brief タイマ取消API
 */
extern void *TIMms_cancelTimer( TIMms_t *handle );

/**
 * @brief タイマ割込みハンドラからコールされる発火処理
 */
extern void TIMms_expire( int over );

/**
 * @} end of addtogroup
 */
#endif //_TIMms_H_
