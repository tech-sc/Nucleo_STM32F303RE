/**************************************************************************//**
 * @file	APL_init.c
 * @brief	アプリ初期起動タスク
 * @version V0.10.0
 * @date	2019/07/04
 * @author	Teru
 */
#include "os.h"
#include "TIMms.h"
#include "APL_init.h"
#include "BSP_LED.h"

/**************************************************************************//**
 * @brief	定数定義
 */

/**************************************************************************//**
 * @brief	関数定義
 */
static void tmX_expire( void *handle );

/**************************************************************************//**
 * @brief	変数定義
 */
osMutexHandle_t     expire_mutex;
TIMms_t				tm1;

/**************************************************************************//**
 * @brief	初期起動タスクを生成する
 */
void APL_init( void )
{
	int		retv;
	xTaskHandle		handle;
	printf("%s\n", __FUNCTION__);

	TIMms_initTimer();

    expire_mutex = osMutex_create();

	retv = osTask_create( &APL_initTask, "APL_init", APLinit_STACKSZ/4,
						NULL, APLinitTASK_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);
}

/**************************************************************************//**
 * @brief	初期起動タスク
 * @param[in]	arg		タスク起動パラメータ
 */
void APL_initTask( void *arg )
{
	TIMms_reqTimer( 500, &tmX_expire, &tm1 );

    while( 1 ){
        osMutex_take( expire_mutex, portMAX_DELAY );
	}
}

/**************************************************************************/
static void tmX_expire( void *handle )
{
	//LOG_write( LOG_DEBUG, __LINE__, 0x0010, 2, TAG_STR, "500ms Expire" );

	BSP_LED_toggle();
	TIMms_reqTimer( 500, &tmX_expire, handle );
}
