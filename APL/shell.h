/**
 * @file  shell.h
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
#ifndef _SHELL_H_
#define _SHELL_H_

/**
 * @page SHELL_title �R�}���h�V�F���@�\
 *
 * @section SHELL_api API�ꗗ
 * | API��                   | �@�\            |
 * |:----------------------- |:--------------- |
 * | shell_init()            | �����ݒ�        |
 * | shell_registerCommand() | CLI�R�}���h�o�^ |
 *
 * @section SHELL_usage �g�p���@
 * - �\�߈�x���� shell_init() ���Ăяo���B
 */

/**
 * @addtogroup GROUP_SHELL �R�}���h�V�F���@�\.
 * @{
 */

/** �L�[���͂ɂ��オ���Ă���ESC�V�[�P���X */
#define ESC_2ND				'['			/**< ESC�L�[�Q������ */
#define ALLOW_UP			'A'			/**< ���L�[:ESC+[+A */
#define ALLOW_DOWN			'B'			/**< ���L�[:ESC+[+B */
#define ALLOW_RIGHT			'C'			/**< ���L�[:ESC+[+C */
#define ALLOW_LEFT			'D'			/**< ���L�[:ESC+[+D */
#define ESCPLUS_HOME		1			/**< HOME�L�[:ESC+[+1+~ */
#define ESCPLUS_INS			2			/**< INS�L�[:ESC+[+2+~ */
#define ESCPLUS_END			4			/**< HOME�L�[:ESC+[+4+~ */
#define ESCPLUS_PGUP		5			/**< PageUp�L�[:ESC+[+5+~ */
#define ESCPLUS_PGDN		6			/**< PageDn�L�[:ESC+[+6+~ */
#define ESCPLUS_F1			11			/**< F1�L�[:ESC+[+11+~ */
#define ESCPLUS_F2			12			/**< F2�L�[:ESC+[+12+~ */
#define ESCPLUS_F3			13			/**< F3�L�[:ESC+[+13+~ */
#define ESCPLUS_F4			14			/**< F4�L�[:ESC+[+14+~ */
#define ESCPLUS_F5			15			/**< F5�L�[:ESC+[+15+~ */
#define ESCPLUS_F6			17			/**< F6�L�[:ESC+[+17+~ */
#define ESCPLUS_F7			18			/**< F7�L�[:ESC+[+18+~ */
#define ESCPLUS_F8			19			/**< F8�L�[:ESC+[+19+~ */
#define ESCPLUS_F9			20			/**< F9�L�[:ESC+[+20+~ */
#define ESCPLUS_F10			21			/**< F10�L�[:ESC+[+21+~ */
#define ESCPLUS_F11			23			/**< F10�L�[:ESC+[+23+~ */
#define ESCPLUS_F12			24			/**< F10�L�[:ESC+[+24+~ */

/** �L�[�R�[�h�i�G�~�����[�g�R�[�h�j */
#define KEY_INS				0x07		///< ASCII�R�[�h:INS�L�[
#define KEY_BS				0x08		///< ASCII�R�[�h:BS�L�[
#define KEY_TAB				0x09		///< ASCII�R�[�h:TAB�L�[
#define KEY_LF				0x0a		///< ASCII�R�[�h:LF
#define KEY_CR				0x0d		///< ASCII�R�[�h:CR
#define KEY_ESC				0x1b		///< ASCII�R�[�h:ESC�L�[
#define KEY_HOME			0x11		///< ASCII�R�[�h:HOME�L�[
#define KEY_END				0x12		///< ASCII�R�[�h:END�L�[
#define KEY_PGUP			0x13		///< ASCII�R�[�h:PageUp�L�[
#define KEY_PGDN			0x14		///< ASCII�R�[�h:PageDown�L�[
#define KEY_UP				0x1c		///< ASCII�R�[�h:UP�L�[
#define KEY_DOWN			0x1d		///< ASCII�R�[�h:DOWN�L�[
#define KEY_RIGHT			0x1e		///< ASCII�R�[�h:RIGHT�L�[
#define KEY_LEFT			0x1f		///< ASCII�R�[�h:LEFT�L�[
#define KEY_DEL				0x7f		///< ASCII�R�[�h:DEL�L�[

/** �o�͂���ESC�V�[�P���X */
#define ESC_SAVE			"\x1b[s"		/**< �J�[�\���ʒu�ƕ\���������Z�[�u���� */
#define ESC_LOAD			"\x1b[u"		/**< �J�[�\���ʒu�ƕ\�����������[�h���� */

 
/**
 * @brief �R�}���h�֐��v���g�^�C�v
 */
typedef int (*SHELL_COMMAND_cb)( int argc, char **argv );

/**
 * @brief �R�}���h�o�^�f�[�^�^.
 */
typedef struct SHELL_COMMAND_t	{
	/// �R�}���h����
	char					*pCommand;
	/// �R�}���h�֐�
	SHELL_COMMAND_cb		pFunc;
	/// �w���v��
	char					*pHelp;
	/// �����f�[�^�F�R�}���h���X�g
	struct SHELL_COMMAND_t	*pNext;
}SHELL_COMMAND_t;


/**
 * @brief �����ݒ�API
 */
extern void shell_init( void );

/**
 * @brief �R�}���h�o�^API
 */
extern int shell_registerCommand( SHELL_COMMAND_t *p_item );

#endif /*_SHELL_H_*/
