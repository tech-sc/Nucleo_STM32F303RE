/************************************************************************************//**
 * @file UART.c
 * @brief RS232C���x���ϊ�����UART����API
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
/**
 * @addtogroup GROUP_UART
 * @{
 */
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_DMA.h"
#include "stm32f1xx_hal_UART.h"
#include "stm32f1xx_ll_usart.h"
#include "UART.h"


#define UART				USART2		/**< �g�p����y���t�F���� */

#define UART_TX_PIN			GPIO_PIN_2
#define UART_TX_GPIO_PORT	GPIOA
#define UART_RX_PIN			GPIO_PIN_3
#define UART_RX_GPIO_PORT	GPIOA


/* HAL API�n���h�� */
static UART_HandleTypeDef	uart_HandleStruct = {0};

/* UART��M�҂��Z�}�t�H */
static xSemaphoreHandle	uart_RxSemaph;

/* �ʐM���[�h */
static uint16_t		uart_TransMode;

/* UART��M�o�b�t�@ */
#define UART_RXBUFF_SIZE	16
static uint8_t		uart_RxBuff[ UART_RXBUFF_SIZE ];
static uint8_t		*uart_RxBuffWt_p;			/* �o�b�t�@�������݃|�C���^ */
static uint8_t		*uart_RxBuffRd_p;			/* �o�b�t�@�ǂݏo���|�C���^ */


/**************************************************************************//**
 * @brief	UART�R���g���[�������ݒ�.
 * @retval	0	Success
 * @retval	-1	Error
 */
static int UART_config( void )
{
	GPIO_InitTypeDef	gpio_init_struct = {0};

	/* Peripheral clock enable */
	if( __HAL_RCC_USART2_IS_CLK_DISABLED() ) {
		__HAL_RCC_USART2_CLK_ENABLE();
	}
	/* GPIO Ports Clock Enable */
	if( __HAL_RCC_GPIOA_IS_CLK_DISABLED() ) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}

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
	return 0;
}

/**************************************************************************//**
 * @brief	UART�h���C�o�����ݒ�
 * @retval	0	Success
 * @retval	-1	Error
 */
int UART_init( void )
{
	/* �����Ǘ��f�[�^������ */
	vSemaphoreCreateBinary( uart_RxSemaph );
	xSemaphoreTake( uart_RxSemaph, portMAX_DELAY );
	uart_RxBuffWt_p = &uart_RxBuff[ 0 ];
	uart_RxBuffRd_p = uart_RxBuffWt_p;

	/* set default mode */
	UART_setMode( ASCII_MODE );

	/* UART�y���t�F���������ݒ� */
	return UART_config();
}

/**************************************************************************//**
 * @brief	���[�h��ݒ肷��
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
 * @brief	���[�h���擾����.
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
void USART2_IRQHandler( void )
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
 * @brief	UART����P�o�C�g�擾����.
 * @return	�擾�����f�[�^.
 * @note	UART��M�o�b�t�@����P�o�C�g�f�[�^�����o��.
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
 * @brief	UART�ɂP�o�C�g�o�͂���.
 * @param[in]	c	�o�͂���f�[�^.
 * @return	�o�͂����f�[�^.
 */
int UART_putchar( char c )
{
	while( ! LL_USART_IsActiveFlag_TXE(UART) );

	LL_USART_TransmitData8( UART, c );
	return c;
}

/**************************************************************************//**
 * @brief	UART�Ɉ���ptr�̕�������o�͂���
 * @param[in]	ptr	�o�͂��镶����
 * @retval	0		�o�͐���
 * @retval	-1		�G���[
 * @note	������I�[��������������o�͂���
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
 * @brief	UART�Ɉ���ptr�̕�������o�͂���
 * @param[in]	ptr	�o�͂��镶����
 * @retval	0		�o�͐���
 * @retval	-1		�G���[
 * @note	������I�[��\0�͉��s�R�[�h�ɕϊ����ďo�͂���
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
