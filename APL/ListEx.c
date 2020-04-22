/**
 * @file  ListEx.c
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
#include <stdio.h>
#include "ListEx.h"


void List_init( ListHdr_t *p_top )
{
	p_top->p_next = NULL;
	p_top->p_prev = NULL;
	//p_top->p_content = NULL;
}

void List_add( ListHdr_t *p_top, ListHdr_t *p_obj )
{
	p_obj->p_next = NULL;

	if( p_top->p_prev == NULL ) {
		p_obj->p_prev = p_top;
		p_top->p_next = p_obj;
		p_top->p_prev = p_obj;
	} else {
		p_obj->p_prev = p_top->p_prev;
		p_top->p_prev->p_next = p_obj;
		p_top->p_prev = p_obj;
	}
}

void List_remove( ListHdr_t *p_top, ListHdr_t *p_obj )
{
	p_obj->p_prev->p_next = p_obj->p_next;
	if( p_top->p_next == NULL ) {
		p_top->p_prev = NULL;
	} else if( p_top->p_prev == p_obj ) {
		p_top->p_prev = p_obj->p_prev;
	} else {
		p_obj->p_next->p_prev = p_obj->p_prev;
	}
}

ListHdr_t *List_find( ListHdr_t *p_top, ListHdr_t *p_obj )
{
	while( p_top != NULL ) {
		if( p_top == p_obj ) {
			return p_top;
		}
		p_top = p_top->p_next;
	}
	return NULL;
}

ListHdr_t *List_getNext( ListHdr_t *p_obj )
{
	return p_obj->p_next;
}

#define LIST_MAX	11
void List_test( void )
{
	ListHdr_t	top;
	ListHdr_t	item[LIST_MAX];
	ListHdr_t	*ptr;
	int		i;

	List_init( &top );
	// add all
	for( i=0; i < LIST_MAX -1; i++ ) {
		//item[i].p_content = (void*)(i+1);
		List_add( &top, &item[i] );
	}

	// delete top
	ptr = List_getNext( &top );
	List_remove( &top, ptr );

	// delete mid
	ptr = List_getNext( &top );
	ptr = List_getNext( ptr );
	List_remove( &top, ptr );

	// delete bottom
	//item[LIST_MAX -1].p_content = (void*)LIST_MAX;
	List_add( &top, &item[LIST_MAX -1] );
	List_remove( &top, &item[LIST_MAX -1] );

	// delete all
	ptr = List_getNext( &top );
	while( ptr != NULL ) {
		List_remove( &top, ptr );
		ptr = List_getNext( ptr );
	}
}
