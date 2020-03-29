/**************************************************************************//**
 * @file	UART.h
 * @brief RS232Cレベル変換向けUART制御API.
 *
 * @author  Teru
 * @date    2014/01/12
 * @version Rev1.02
 * @par 変更履歴:
 * - Rev1.01: 2014/01/12: 初期リリース版.
 * - Rev1.10: 2019/06/19: STM32 HALドライバ使用.
 *
 * @copyright 2014 Emb-se.com.
 */
#ifndef _UART_H_
#define _UART_H_

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @page PAGE_UART UART制御.
 *
 * @section SEC1_UART UART制御API.
 * |APIs     |機能名称|
 * |:--------|:------ |
 * |UART_init() | UART初期設定 |
 * |UART_setMode() | Text/Binaryモード設定 |
 * |UART_getMode() | モード(Text/Binary)取得 |
 * |UART_getchar() | 一文字出力 |
 * |UART_putchar() | 一文字入力 |
 * |UART_puts() | 文字列改行付き出力 |
 * |UART_putstr() | 文字列出力 |
 */
/**
 * @addtogroup GROUP_UART UART制御.
 * @brief UART制御APIを提供する.<br>
 * @{
 */

#define CR_CODE				'\r'
#define LF_CODE				'\n'

#define ASCII_MODE			0		/**< Text mode */
#define BINARY_MODE			1		/**< Binary mode */


extern int  UART_init( void );
extern int  UART_setMode( uint16_t mode );
extern uint16_t  UART_getMode( void );
extern int  UART_getchar( void );
extern int  UART_putchar( char c );
extern int  UART_puts( char *ptr );
extern int  UART_putstr( char *ptr );

/**
 * @} end of addtogroup
 */
#endif /*_UART_H_*/
