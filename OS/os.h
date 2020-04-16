/************************************************************************************//**
 * @file os.h
 * @brief OSインタフェース定義
 *
 * @author		Teru
 * @date		2020/04/16
 * @version 	Rev0.10
 *
 * @par 変更履歴:
 * - Rev0.01: 2019/07/04: 新規作成.
 * - Rev0.02: 2019/09/22: vSemaphoreCreateBinary→xSemaphoreCreateBinary変更.
 * - Rev0.10: 2020/04/16: osTaskHandle_t→osTask_hなどハンドル型名変更.
 *                        osRetVal→osErrorに型名変更.
 * @note OSは、FreeRTOS v7.6.0 を使用する。
 *
 * @copyright	2019-20 Emb-se.com.
 */
#ifndef _OS_H_
#define _OS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#define osKernel_start			vTaskStartScheduler
#define osEnterCritical			portENTER_CRITICAL
#define osExitCritical			portEXIT_CRITICAL

#define osTask_create			xTaskCreate

#define osQue_create			xQueueCreate
#define osQue_send				xQueueSend
#define osQue_sendISR			xQueueSendFromISR
#define osQue_peek				xQueuePeek
#define osQue_receive			xQueueReceive

#define osMutex_create			xSemaphoreCreateBinary
#define osMutex_take			xSemaphoreTake
#define osMutex_give			xSemaphoreGive
#define osMutex_giveISR			xSemaphoreGiveFromISR

#define osMem_malloc			pvPortMalloc
#define osMem_free				pvPortFree

typedef xTaskHandle				osTask_h;
typedef xQueueHandle			osQue_h;
typedef xSemaphoreHandle		osMutex_h;

typedef portBASE_TYPE			osError;

#define osBLOCKING				portMAX_DELAY
#define osPOLLING				(0)
#endif //_OS_H_
