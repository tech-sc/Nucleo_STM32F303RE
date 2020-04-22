/**
 * @file  ListEx.h
 * @brief 双方向リスト機能.
 * 
 * 双方向リスト機能をAPLに提供する.
 * 
 * @author		Teru
 * @date		2020/04/21
 * @version		Rev0.10
 * 
 * @par 変更履歴:
 * - Rev0.10: 2019/12/21: 新規作成.
 * 
 * @copyright	2020 Emb-se.com.
 */
/**
 * @addtogroup GROUP_ListEx 双方向リスト機能.
 * @{
 */
#ifndef _ListEx_H_
#define _ListEx_H_

typedef struct ListHdr_t {
	struct ListHdr_t *p_next;
	struct ListHdr_t *p_prev;
	//void *p_content;
} ListHdr_t;

extern void List_init( ListHdr_t *p_top );
extern void List_add( ListHdr_t *p_top, ListHdr_t *p_obj );
extern void List_remove( ListHdr_t *p_top, ListHdr_t *p_obj );

extern ListHdr_t *List_find( ListHdr_t *p_top, ListHdr_t *p_obj );
extern ListHdr_t *List_getNext( ListHdr_t *p_obj );

/**
 * @brief テストコード
 */
extern void List_test( void );

/**
 * @} end of addtogroup
 */
#endif //_ListEx_H_
