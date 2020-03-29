/**************************************************************************//**
 * @file	UART.h
 * @brief RS232C���x���ϊ�����UART����API.
 *
 * @author  Teru
 * @date    2014/01/12
 * @version Rev1.02
 * @par �ύX����:
 * - Rev1.01: 2014/01/12: ���������[�X��.
 * - Rev1.10: 2019/06/19: STM32 HAL�h���C�o�g�p.
 *
 * @copyright 2014 Emb-se.com.
 */
#ifndef _UART_H_
#define _UART_H_

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @page PAGE_UART UART����.
 *
 * @section SEC1_UART UART����API.
 * |APIs     |�@�\����|
 * |:--------|:------ |
 * |UART_init() | UART�����ݒ� |
 * |UART_setMode() | Text/Binary���[�h�ݒ� |
 * |UART_getMode() | ���[�h(Text/Binary)�擾 |
 * |UART_getchar() | �ꕶ���o�� |
 * |UART_putchar() | �ꕶ������ |
 * |UART_puts() | ��������s�t���o�� |
 * |UART_putstr() | ������o�� |
 */
/**
 * @addtogroup GROUP_UART UART����.
 * @brief UART����API��񋟂���.<br>
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
