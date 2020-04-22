/************************************************************************************//**
 * @file UART.c
 * @brief RS232Cレベル変換向けUART制御API
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
/**
 * @addtogroup GROUP_UART
 * @{
 */
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"
#include "stm32f3xx_hal_rcc.h"
#include "stm32f3xx_hal_DMA.h"
#include "stm32f3xx_hal_UART.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_rcc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "UART.h"


#ifdef STM32F103xB
#define UART				USART2		/**< 使用するペリフェラル */

#define UART_TX_PIN			GPIO_PIN_2
#define UART_TX_GPIO_PORT	GPIOA
#define UART_RX_PIN			GPIO_PIN_3
#define UART_RX_GPIO_PORT	GPIOA
#endif
#ifdef STM32F303xE
#define UART				USART1		/**< 使用するペリフェラル */

#define UART_TX_PIN			GPIO_PIN_4
#define UART_TX_GPIO_PORT	GPIOC
#define UART_RX_PIN			GPIO_PIN_5
#define UART_RX_GPIO_PORT	GPIOC
#endif


/* HAL APIハンドル */
static UART_HandleTypeDef	uart_HandleStruct = {0};

/* UART受信待ちセマフォ */
static xSemaphoreHandle	uart_RxSemaph;

/* 通信モード */
static uint16_t		uart_TransMode;

/* UART受信バッファ */
#define UART_RXBUFF_SIZE	16
static uint8_t		uart_RxBuff[ UART_RXBUFF_SIZE ];
static uint8_t		*uart_RxBuffWt_p;			/* バッファ書き込みポインタ */
static uint8_t		*uart_RxBuffRd_p;			/* バッファ読み出しポインタ */


/**************************************************************************//**
 * @brief	UARTコントローラ初期設定.
 * @retval	0	Success
 * @retval	-1	Error
 */
static int UART_config( void )
{
	GPIO_InitTypeDef	gpio_init_struct = {0};

#ifdef STM32F103xB
	/* Clock Enable */
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX 
	*/
	gpio_init_struct.Pin   = UART_TX_PIN;
	gpio_init_struct.Mode  = GPIO_MODE_AF_PP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init( UART_TX_GPIO_PORT, &gpio_init_struct );

	gpio_init_struct.Pin   = UART_RX_PIN;
	gpio_init_struct.Mode  = GPIO_MODE_INPUT;
	gpio_init_struct.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init( UART_RX_GPIO_PORT, &gpio_init_struct );

	uart_HandleStruct.Instance          = UART;
	uart_HandleStruct.Init.BaudRate     = 115200;
	uart_HandleStruct.Init.WordLength   = UART_WORDLENGTH_8B;
	uart_HandleStruct.Init.StopBits     = UART_STOPBITS_1;
	uart_HandleStruct.Init.Parity       = UART_PARITY_NONE;
	uart_HandleStruct.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	uart_HandleStruct.Init.Mode         = UART_MODE_TX_RX;
	uart_HandleStruct.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init( &uart_HandleStruct ) != HAL_OK ) {
		return -1;
	}

	/* USART2 interrupt Init */
	__HAL_UART_ENABLE_IT( &uart_HandleStruct, UART_IT_RXNE );
	HAL_NVIC_SetPriority( USART2_IRQn, 5, 0 );
	HAL_NVIC_EnableIRQ( USART2_IRQn );
#endif
#ifdef STM32F303xE
	/* Peripheral clock enable */
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
	/* Clock Enable */
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/**USART1 GPIO Configuration
    PC4     ------> USART1_TX
    PC5     ------> USART1_RX
    */
	gpio_init_struct.Pin = UART_TX_PIN|UART_RX_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(UART_TX_GPIO_PORT, &gpio_init_struct);

	uart_HandleStruct.Instance = UART;
	uart_HandleStruct.Init.BaudRate = 38400;
	uart_HandleStruct.Init.WordLength = UART_WORDLENGTH_8B;
	uart_HandleStruct.Init.StopBits = UART_STOPBITS_1;
	uart_HandleStruct.Init.Parity = UART_PARITY_NONE;
	uart_HandleStruct.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_HandleStruct.Init.Mode = UART_MODE_TX_RX;
	uart_HandleStruct.Init.OverSampling = UART_OVERSAMPLING_16;
	uart_HandleStruct.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart_HandleStruct.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if( HAL_UART_Init(&uart_HandleStruct) != HAL_OK) {
		return -1;
	}

	/* USART1 interrupt Init */
	__HAL_UART_ENABLE_IT( &uart_HandleStruct, UART_IT_RXNE );
	HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif
	return 0;
}

/**************************************************************************//**
 * @brief	UARTドライバ初期設定
 * @retval	0	Success
 * @retval	-1	Error
 */
int UART_init( void )
{
	/* 内部管理データ初期化 */
	vSemaphoreCreateBinary( uart_RxSemaph );
	xSemaphoreTake( uart_RxSemaph, portMAX_DELAY );
	uart_RxBuffWt_p = &uart_RxBuff[ 0 ];
	uart_RxBuffRd_p = uart_RxBuffWt_p;

	/* set default mode */
	UART_setMode( ASCII_MODE );

	/* UARTペリフェラル初期設定 */
	return UART_config();
}

/**************************************************************************//**
 * @brief	モードを設定する
 * @param[in]	mode	ASCII_MODE/BINARY_MODE
 * @retval	0	Success
 * @retval	-1	Error
 */
int UART_setMode( uint16_t mode )
{
	if( (mode == ASCII_MODE)||(mode == BINARY_MODE) ) {
		uart_TransMode = mode;
		return 0;
	}
	return -1;
}

/**************************************************************************//**
 * @brief	モードを取得する.
 * @retval	ASCII_MODE
 * @retval	BINARY_MODE
 */
uint16_t UART_getMode( void )
{
	return uart_TransMode;
}

/**************************************************************************//**
 * @brief	This function handles USART2 global interrupt.
 */
#ifdef STM32F103xB
void USART2_IRQHandler( void )
#endif
#ifdef STM32F303xE
void USART1_IRQHandler( void )
#endif
{
	portBASE_TYPE	each_dispatch, dispatch;

	each_dispatch = pdFALSE;
	dispatch	  = pdFALSE;

	if( uart_TransMode == ASCII_MODE ) {
		if( LL_USART_IsActiveFlag_RXNE(UART) ) {
			/* RXNE pending bit is cleared by a read to the USART_RDR register */
			*uart_RxBuffWt_p++   = LL_USART_ReceiveData8( UART );
			if( uart_RxBuffWt_p >= &uart_RxBuff[ UART_RXBUFF_SIZE ] ) {
				uart_RxBuffWt_p  = &uart_RxBuff[ 0 ];
			}

			xSemaphoreGiveFromISR( uart_RxSemaph, &each_dispatch );
			if( each_dispatch ) {
				dispatch = each_dispatch;
			}
		}
		portEND_SWITCHING_ISR( dispatch );
	}
}

/**************************************************************************//**
 * @brief	UARTから１バイト取得する.
 * @return	取得したデータ.
 * @note	UART受信バッファから１バイトデータを取り出す.
 */
int UART_getchar( void )
{
	int		c = 0;

	xSemaphoreTake( uart_RxSemaph, portMAX_DELAY );

	if( uart_RxBuffRd_p != uart_RxBuffWt_p ) {
		c = *uart_RxBuffRd_p++;
		if( uart_RxBuffRd_p >= &uart_RxBuff[ UART_RXBUFF_SIZE ] ) {
			uart_RxBuffRd_p = &uart_RxBuff[ 0 ];
		}
	}
	return c;
}

/**************************************************************************//**
 * @brief	UARTに１バイト出力する.
 * @param[in]	c	出力するデータ.
 * @return	出力したデータ.
 */
int UART_putchar( char c )
{
	while( ! LL_USART_IsActiveFlag_TXE(UART) );

	LL_USART_TransmitData8( UART, c );
	return c;
}

/**************************************************************************//**
 * @brief	UARTに引数ptrの文字列を出力する
 * @param[in]	ptr	出力する文字列
 * @retval	0		出力成功
 * @retval	-1		エラー
 * @note	文字列終端を除く文字列を出力する
 */
int UART_putstr( char *ptr )
{
	if( ptr == NULL ) {
		return -1;
	}

	while( *ptr != '\0' ) {
		UART_putchar( *ptr );
		ptr ++;
	}
	return 0;
}

/**************************************************************************//**
 * @brief	UARTに引数ptrの文字列を出力する
 * @param[in]	ptr	出力する文字列
 * @retval	0		出力成功
 * @retval	-1		エラー
 * @note	文字列終端の\0は改行コードに変換して出力する
 */
int UART_puts( char *ptr )
{
	if( UART_putstr( ptr ) != 0 ) {
		return -1;
	}

	UART_putchar( CR_CODE );
	UART_putchar( LF_CODE );
	return 0;
}
/**
 * @} end of addtogroup
 */
