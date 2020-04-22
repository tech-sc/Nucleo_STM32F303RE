/**
 * @file  shell.c
 * @brief �R�}���h�V�F���@�\.
 * 
 * �R�}���h�E���C���E�C���^�t�F�[�X�ɃR�}���h�E�V�F���@�\��񋟂���.
 * 
 * @author      Teru
 * @date        2014/01/12
 * @version     Rev0.10
 * 
 * @par �ύX����:
 * - Rev0.01: 2014/01/12: �V�K�쐬.
 * - Rev0.02: 2019/08/13: Doxygen�Ή�.
 * 
 * @copyright   2014-19 Emb-se.com.
 */
/**
 * @addtogroup GROUP_SHELL �R�}���h�V�F���@�\.
 * @{
 */
#include <stdio.h>
#include <string.h>
#include "stm32_assert.h"
#include "FreeRTOS.h"
#include "task.h"
#include "UART.h"
#include "shell.h"

/**
 * @brief �萔��`
 */
/// ���s�R�[�h���[�h�FCR+LF
#undef  CRLF_MODE
/// ���s�R�[�h���[�h�FCR
#undef  CR_MODE
/// ���s�R�[�h���[�h�FLF
#define LF_MODE

/// �v�����v�g
#define SHELL_PROMPT		"sh #"

#if defined(CRLF_MODE)
  #define SHELL_CR				"\r\n"
  #define SHELL_NOTFOUND_ERR	"\r\ncommand not found.\r\n"
  #define SHELL_COMMAND_ERR		"\r\ncommand error.\r\n"
#elif defined(CR_MODE)
  #define SHELL_CR				"\n"
  #define SHELL_NOTFOUND_ERR	"\ncommand not found.\n"
  #define SHELL_COMMAND_ERR		"\ncommand error.\n"
#else //LF_MODE
  #define SHELL_CR				"\r"
  #define SHELL_NOTFOUND_ERR	"\rcommand not found.\r"
  #define SHELL_COMMAND_ERR		"\rcommand error.\r"
#endif //LF_MODE


/**
 * @brief �֐���`
 */
/** �V�F�����o�͐�f�o�C�XI/O API��` */
/// �P��������(���͐�f�o�C�X�FUART)
#define MonIO_getchar		UART_getchar
/// ������+CR�o��(�o�͐�f�o�C�X�FUART)
#define MonIO_puts			UART_puts
/// ������o��(�o�͐�f�o�C�X�FUART)
#define MonIO_putstr		UART_putstr

/** �����֐� */
/// �v�����v�g�o��
#define shell_promt()		MonIO_putstr( SHELL_PROMPT )
/// �v�����v�g+CR�o��
#define shell_cr_promt()	MonIO_putstr( SHELL_CR SHELL_PROMPT )

static void shell_task( void *arg );
static int  shell_help( int argc, char **argv );
static int  shell_core( void );
static int  shell_esc( void );
static SHELL_COMMAND_t *shell_find_command( SHELL_COMMAND_t *p_list, char *str, int len );
static void shell_execute( int argc, char **argv );
static int  shell_arglist( char **argv, char *str );


/**
 * @brief �ϐ���`
 */
/// �V�F���R�A:�o�b�t�@�T�C�Y
#define SHELL_CORE_BUFF_SIZE		384
/// �V�F���R�A:�o�b�t�@
static char		shell_CoreBuff[ SHELL_CORE_BUFF_SIZE ];
/// �V�F���R�A:�J�[�\���ʒu�|�C���^
static char		*shell_CoreBuffCursol_p;
/// �V�F���R�A:�R�}���h������
static int16_t		shell_CoreBuff_len;

/// �e���|�����o�b�t�@
static char		shell_tempBuff[ 20 ];
/// �e���|�����o�b�t�@������
static int16_t		shell_tempBuff_len;

/// CLI�R�}���h�������X�g��
#define SHELL_ARGV_SIZE				16
/// CLI�R�}���h�������X�g
static char	*shell_argv[ SHELL_ARGV_SIZE ];

/// CLI�R�}���h�e�[�u��
static SHELL_COMMAND_t	shell_CommandTbl = {
	"help", shell_help, "\tshow entry command lists." SHELL_CR "\tshell version 1.00", NULL
};


/**************************************************************************//**
 * @brief  �����ݒ�API
 *
 * �R�}���h�V�F���@�\�̏����ݒ���s��.
 */
void shell_init( void )
{
	portBASE_TYPE	retv;
	xTaskHandle		handle;

	UART_init();

	shell_CoreBuff_len     = 0;
	shell_CoreBuffCursol_p = shell_CoreBuff;

	retv = xTaskCreate( &shell_task, "Shell", shell_STACKSZ/4,	// stack size=64*4=256byte
						NULL, 3 | portPRIVILEGE_BIT, &handle );
	configASSERT(retv == pdPASS);
}

/**************************************************************************//**
 * @brief  CLI�R�}���h�V�F���^�X�N
 * @param[in] arg  �^�X�N�N���p�����[�^
 */
static void shell_task( void *arg )
{
	int			argc;

	shell_cr_promt();

	for( ;; ) {
		if( UART_getMode() == ASCII_MODE )
		{
			/** �e�L�X�g���[�h */
			shell_core();
			if( shell_CoreBuff_len == 0 ) {
				shell_promt();
			} else {
				argc = shell_arglist( shell_argv, (char*)shell_CoreBuff );
				/** CLI�R�}���h���s */
				shell_execute( argc, shell_argv );
				shell_cr_promt();
			}
		} else {
			/* �o�C�i�����[�h */
		}
	}
}

/**************************************************************************//**
 * @brief  CLI�R�}���h�V�F���R�A
 * @retval	0	Success
 */
static int shell_core( void )
{
	SHELL_COMMAND_t		*p_list = NULL;
	short		loop = 1;
	uint8_t		ch;

	shell_tempBuff_len = 0;
	shell_CoreBuffCursol_p  = shell_CoreBuff;
	*shell_CoreBuffCursol_p = '\0';
	shell_CoreBuff_len      = 0;

	while( loop )
	{
		MonIO_putstr( ESC_SAVE );

		ch = MonIO_getchar();
		switch( ch )
		{
		  case KEY_ESC:
			if( ! shell_esc() ) {
				MonIO_putstr( ESC_LOAD );
			}
			break;
#if 0
		  case KEY_BS:
			if( shell_CoreBuff_len > 0 ) {
				shell_CoreBuff_len--;
				shell_CoreBuffCursol_p--;
				shell_CoreBuff[ shell_CoreBuff_len ] = '\0';
			} else {
				MonIO_putstr( ESC_LOAD );
			}
			break;
		  case KEY_DEL:
			if( shell_CoreBuff_len > 0 ) {
				shell_CoreBuff_len--;
				memcpy( shell_CoreBuffCursol_p, shell_CoreBuffCursol_p+1, shell_CoreBuff_len );
			} else {
				MonIO_putstr( ESC_LOAD );
			}
			break;
#endif
		  case KEY_TAB:
			if( shell_tempBuff_len == 0 ) {
				strcpy( shell_tempBuff, shell_CoreBuff );
				shell_tempBuff_len = shell_CoreBuff_len;
			}
			p_list = shell_find_command( p_list, shell_tempBuff, shell_tempBuff_len );
			if( p_list != NULL ) {
				// �J�[�\�������� shell_tempBuff_len �ړ�����
				;
				strcpy( shell_CoreBuff, p_list->pCommand );
				shell_CoreBuff_len = strlen( shell_CoreBuff );
				MonIO_putstr( shell_CoreBuff );
			} else {
				MonIO_putstr( ESC_LOAD );
			}
			break;

#if defined(CR_MODE)||defined(CRLF_MODE)
		  case KEY_CR:
			shell_CoreBuff[ shell_CoreBuff_len ] = '\0';
#if defined(CR_MODE)
			loop = 0;
			break;
#endif //CR_MODE
#endif //CR_MODE||CRLF_MODE

#if defined(LF_MODE)||defined(CRLF_MODE)
		  case KEY_LF:
			shell_CoreBuff[ shell_CoreBuff_len ] = '\0';
			loop = 0;
			break;
#endif //LF_MODE||CRLF_MODE

		  default:
			if( shell_CoreBuff_len < SHELL_CORE_BUFF_SIZE -1 ) {
				if( shell_CoreBuffCursol_p - shell_CoreBuff == shell_CoreBuff_len ) {
					shell_CoreBuff_len++;
				}
				*shell_CoreBuffCursol_p = ch;
				shell_CoreBuffCursol_p++;
			}
			break;
		}
	}
	return 0;
}

/**************************************************************************//**
 * @brief  ESC�V�[�P���X����
 * @retval 1  �L�[�����ς�
 * @retval 0  �s���L�[(������)
 */
static int shell_esc( void )
{
	char		ch;
	char		virtual_code = 0;
	char		numeric = 0;
	int			retv = 0;

	while( virtual_code == 0 )
	{
		ch = MonIO_getchar();
		switch( ch )
		{
		  case ESC_2ND:
			break;

		  case ALLOW_LEFT:
			if( shell_CoreBuffCursol_p > shell_CoreBuff ) {
				shell_CoreBuffCursol_p --;
				retv = 1;
			}
			virtual_code = KEY_LEFT;
			break;
		  case ALLOW_RIGHT:
			if( shell_CoreBuffCursol_p - shell_CoreBuff < shell_CoreBuff_len ) {
				shell_CoreBuffCursol_p ++;
				retv = 1;
			}
			virtual_code = KEY_RIGHT;
			break;

		  default:
			if( (ch >= '0')&&(ch <= '9') ) {
				numeric *= 10;
				numeric += ch - '0';
			} else if( ch == '~' ) {
				switch( numeric )
				{
				  case ESCPLUS_INS:
					virtual_code = KEY_INS;
					break;
				  case ESCPLUS_HOME:
					virtual_code = KEY_HOME;
					break;
				  case ESCPLUS_END:
					virtual_code = KEY_END;
					break;
				  case ESCPLUS_PGUP:
					virtual_code = KEY_PGUP;
					break;
				  case ESCPLUS_PGDN:
					virtual_code = KEY_PGDN;
					break;
				  default:
					virtual_code = -1;
					break;
				}
			} else {
				virtual_code = -1;
			}
			break;
		}
	}
	return retv;
}

/**************************************************************************//**
 * @brief  CLI�R�}���h��������
 * @param[in] p_item  �����J�n����R�}���h���
 * @param[in] str  ��������R�}���h������
 * @param[in] len  ��������R�}���h�̕�����
 * @retval NULL  ������Ȃ�
 * @retval !NULL  ������v�����R�}���h�̃R�}���h���
 */
static SHELL_COMMAND_t *shell_find_command( SHELL_COMMAND_t *p_list, char *str, int len )
{
	if( p_list == NULL ) {
		p_list = &shell_CommandTbl;
	}
	while( p_list != NULL ) {
		if( strncmp( p_list->pCommand, str, len ) == 0 ) {
			break;
		}
		p_list = p_list->pNext;
	}
	return p_list;
}

/**************************************************************************//**
 * @brief	�o�^���ꂽCLI�R�}���h�̎��s
 * @param[in]	argc	CLI�R�}���h�����̐�
 * @param[in]	argv	CLI�R�}���h�������X�g
 */
static void shell_execute( int argc, char **argv )
{
	SHELL_COMMAND_t		*p_list;
	int					retv = 0;

	p_list = &shell_CommandTbl;
	while( p_list != NULL ) {
		if( strcmp( p_list->pCommand, argv[0] ) == 0 ) {
			/* �R�}���h���s */
			retv = (p_list->pFunc)( argc, argv );
			break;
		}
		p_list = p_list->pNext;
	}

	if( p_list == NULL ) {
		MonIO_puts( SHELL_NOTFOUND_ERR );
	} else if( retv != 0 ) {
		MonIO_puts( SHELL_COMMAND_ERR );
	}
	MonIO_puts( SHELL_CR );
}

/**************************************************************************//**
 * @brief	CLI�R�}���h�������X�g����
 * @param[out]	argv		CLI�R�}���h�������X�g
 * @param[inout]	str		CLI�R�}���h�s������^�󔒕�����NULL�����ɒu��������
 * @return	���������R�}���h�����̐�
 */
static int shell_arglist( char **argv, char *str )
{
	int			argc = 0;
	char		*top;

	top = str;
	while( *str != '\0' ) {
		if( *str == ' ' ) {
			*str++  = '\0';
			if( (*top != '\0') && (argc < SHELL_ARGV_SIZE -1) ) {
				argc++;
				*argv++ = top;
			}
			top = str;
		} else {
			str++;
		}
	}
	if( (*top != '\0') && (argc < SHELL_ARGV_SIZE -1) ) {
		argc++;
		*argv++ = top;
	}
	*argv = NULL;
	return argc;
}

/**************************************************************************//**
 * @brief	�w���v�R�}���h�i�R�}���h�ꗗ�\���j
 * @param[in]	argc		�R�}���h�����̐�
 * @param[in]	argv		�R�}���h�������X�g
 * @retval	0	Success
 */
static int shell_help( int argc, char **argv )
{
	SHELL_COMMAND_t		*p_list;

	p_list = &shell_CommandTbl;
	while( p_list != NULL ) {
		MonIO_putstr( p_list->pCommand );
		MonIO_puts( p_list->pHelp );

		p_list = p_list->pNext;
	}
	return 0;
}


/**************************************************************************//**
 * @brief �R�}���h�o�^API
 *
 * �R�}���h�V�F���@�\�ɃR�}���h��o�^����.
 * @param[in]	p_item		�o�^����R�}���h��� ���X�^�b�N�̈�NG
 * @retval	0	�o�^OK
 * @retval	-1	�p�����[�^�G���[
 */
int shell_registerCommand( SHELL_COMMAND_t *p_item )
{
	SHELL_COMMAND_t		**p_list;

	if( p_item == NULL ) {
		return -1;
	}
	if( (p_item->pCommand == NULL)||(p_item->pFunc == NULL)||
		(p_item->pHelp == NULL) ) {
		return -1;
	}

	p_item->pNext = NULL;
	p_list = &shell_CommandTbl.pNext;
	while( *p_list != NULL ) {
		if( strcmp( (*p_list)->pCommand, p_item->pCommand ) > 0 ) {
			p_item->pNext = (*p_list)->pNext;
			break;
		}
		p_list = &(*p_list)->pNext;
	}
	*p_list = p_item;
	return 0;
}
