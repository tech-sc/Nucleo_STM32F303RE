/**
 * @file  shell.h
 * @brief コマンドシェル機能.
 * 
 * コマンド・ライン・インタフェースにコマンド・シェル機能を提供する.
 * 
 * @author      Teru
 * @date        2014/01/12
 * @version     Rev0.10
 * 
 * @par 変更履歴:
 * - Rev0.01: 2014/01/12: 新規作成.
 * - Rev0.02: 2019/08/13: Doxygen対応.
 * 
 * @copyright   2014-19 Emb-se.com.
 */
#ifndef _SHELL_H_
#define _SHELL_H_

/**
 * @page SHELL_title コマンドシェル機能
 *
 * @section SHELL_api API一覧
 * | API名                   | 機能            |
 * |:----------------------- |:--------------- |
 * | shell_init()            | 初期設定        |
 * | shell_registerCommand() | CLIコマンド登録 |
 *
 * @section SHELL_usage 使用方法
 * - 予め一度だけ shell_init() を呼び出す。
 */

/**
 * @addtogroup GROUP_SHELL コマンドシェル機能.
 * @{
 */

/** キー入力による上がってくるESCシーケンス */
#define ESC_2ND				'['			/**< ESCキー２文字目 */
#define ALLOW_UP			'A'			/**< ↑キー:ESC+[+A */
#define ALLOW_DOWN			'B'			/**< ↓キー:ESC+[+B */
#define ALLOW_RIGHT			'C'			/**< →キー:ESC+[+C */
#define ALLOW_LEFT			'D'			/**< ←キー:ESC+[+D */
#define ESCPLUS_HOME		1			/**< HOMEキー:ESC+[+1+~ */
#define ESCPLUS_INS			2			/**< INSキー:ESC+[+2+~ */
#define ESCPLUS_END			4			/**< HOMEキー:ESC+[+4+~ */
#define ESCPLUS_PGUP		5			/**< PageUpキー:ESC+[+5+~ */
#define ESCPLUS_PGDN		6			/**< PageDnキー:ESC+[+6+~ */
#define ESCPLUS_F1			11			/**< F1キー:ESC+[+11+~ */
#define ESCPLUS_F2			12			/**< F2キー:ESC+[+12+~ */
#define ESCPLUS_F3			13			/**< F3キー:ESC+[+13+~ */
#define ESCPLUS_F4			14			/**< F4キー:ESC+[+14+~ */
#define ESCPLUS_F5			15			/**< F5キー:ESC+[+15+~ */
#define ESCPLUS_F6			17			/**< F6キー:ESC+[+17+~ */
#define ESCPLUS_F7			18			/**< F7キー:ESC+[+18+~ */
#define ESCPLUS_F8			19			/**< F8キー:ESC+[+19+~ */
#define ESCPLUS_F9			20			/**< F9キー:ESC+[+20+~ */
#define ESCPLUS_F10			21			/**< F10キー:ESC+[+21+~ */
#define ESCPLUS_F11			23			/**< F10キー:ESC+[+23+~ */
#define ESCPLUS_F12			24			/**< F10キー:ESC+[+24+~ */

/** キーコード（エミュレートコード） */
#define KEY_INS				0x07		///< ASCIIコード:INSキー
#define KEY_BS				0x08		///< ASCIIコード:BSキー
#define KEY_TAB				0x09		///< ASCIIコード:TABキー
#define KEY_LF				0x0a		///< ASCIIコード:LF
#define KEY_CR				0x0d		///< ASCIIコード:CR
#define KEY_ESC				0x1b		///< ASCIIコード:ESCキー
#define KEY_HOME			0x11		///< ASCIIコード:HOMEキー
#define KEY_END				0x12		///< ASCIIコード:ENDキー
#define KEY_PGUP			0x13		///< ASCIIコード:PageUpキー
#define KEY_PGDN			0x14		///< ASCIIコード:PageDownキー
#define KEY_UP				0x1c		///< ASCIIコード:UPキー
#define KEY_DOWN			0x1d		///< ASCIIコード:DOWNキー
#define KEY_RIGHT			0x1e		///< ASCIIコード:RIGHTキー
#define KEY_LEFT			0x1f		///< ASCIIコード:LEFTキー
#define KEY_DEL				0x7f		///< ASCIIコード:DELキー

/** 出力するESCシーケンス */
#define ESC_SAVE			"\x1b[s"		/**< カーソル位置と表示属性をセーブする */
#define ESC_LOAD			"\x1b[u"		/**< カーソル位置と表示属性をロードする */

 
/**
 * @brief コマンド関数プロトタイプ
 */
typedef int (*SHELL_COMMAND_cb)( int argc, char **argv );

/**
 * @brief コマンド登録データ型.
 */
typedef struct SHELL_COMMAND_t	{
	/// コマンド名称
	char					*pCommand;
	/// コマンド関数
	SHELL_COMMAND_cb		pFunc;
	/// ヘルプ文
	char					*pHelp;
	/// 内部データ：コマンドリスト
	struct SHELL_COMMAND_t	*pNext;
}SHELL_COMMAND_t;


/**
 * @brief 初期設定API
 */
extern void shell_init( void );

/**
 * @brief コマンド登録API
 */
extern int shell_registerCommand( SHELL_COMMAND_t *p_item );

#endif /*_SHELL_H_*/
