/**************************************************************************//**
 * @file	APL_init.c
 * @brief	�A�v�������N���^�X�N
 * @version V0.10.0
 * @date	2019/07/04
 * @author	Teru
 */
#include "os.h"
#include "TIMms.h"
#include "APL_init.h"
#include "BSP_LED.h"

/**************************************************************************//**
 * @brief	�萔��`
 */

/**************************************************************************//**
 * @brief	�֐���`
 */
static void tmX_expire( void *handle );

/**************************************************************************//**
 * @brief	�ϐ���`
 */
osMutexHandle_t     expire_mutex;
TIMms_t				tm1;

/**************************************************************************//**
 * @brief	�����N���^�X�N�𐶐�����
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
 * @brief	�����N���^�X�N
 * @param[in]	arg		�^�X�N�N���p�����[�^
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
