/************************************************************************************//**
 * @file os.h
 * @brief OSインタフェース定義
 *
 * @author		Teru
 * @date		2019/07/04
 * @version 	Rev0.02
 * @copyright	2019 Emb-se.com.
 *
 * @par 変更履歴:
 * - Rev0.01: 2019/07/04: 新規作成.
 * - Rev0.02: 2019/09/22: vSemaphoreCreateBinary→xSemaphoreCreateBinary変更.
 * @note OSは、FreeRTOS v7.6.0 を使用する。
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

typedef xTaskHandle				osTaskHandle_t;
typedef xQueueHandle			osQueHandle_t;
typedef xSemaphoreHandle		osMutexHandle_t;
typedef portBASE_TYPE			osRetVal_t;

#define osMAX_TIME				portMAX_DELAY
#endif //_OS_H_
