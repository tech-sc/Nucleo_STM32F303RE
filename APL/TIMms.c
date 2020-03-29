/**
 * @file  TIMms.c
 * @brief �~���b�I�[�_�[�^�C�}�@�\.
 * 
 * �~���b�P�ʂŗv���\�ȃ\�t�g�E�F�A�^�C�}��APL�ɒ񋟂���.
 * 
 * @author		Teru
 * @date		2019/07/27
 * @version		Rev0.10
 * @copyright	2019 Emb-se.com.
 * 
 * @par �ύX����:
 * - Rev0.01: 2019/06/27: �V�K�쐬.
 * - Rev0.02: 2019/07/27: Doxygen�Ή���.
 * 
 * @copyright	2019 Emb-se.com.
 */
/**
 * @addtogroup GROUP_TIMms �~���b�I�[�_�[�^�C�}�@�\.
 * @{
 */
#include "TIMER_ms.h"
#include "os.h"
#include "TIMms.h"
//#include "ExtLED.h"		///�v���p.

/// �^�C�}�v�����X�g�|�C���^
static TIMms_t		*TIMms_TopReq;
/// �^�C�}�v�����X�g�|�C���^
static TIMms_t		*TIMms_TailReq;
/// �^�C�}���΃��X�g�|�C���^
static TIMms_t		*TIMms_TopExpire;

/// ���񔭉�tick�l
static TICK16_t		TIMms_NextExpire;

/// �^�C�}���΃^�X�N���N�����邽�߂̓������ϐ�
static osMutexHandle_t	TIMms_ExpireMutex;

#if 0 //LowLevel��TIMER�Ɉڍs����.
/// �p���[�I������̌o��tick
static union {
	struct {
		TICK16_t	lo;
		TICK16_t	hi;
	}tick_w;
	uint32_t	tick_l;
} TIMms_Tick;
#endif

/**
 * @brief �~���b�^�C�}���Ώ����^�X�N.
 * @param[in]	arg	 ���g�p.
 */
static void TIMms_task( void *arg )
{
	TIMms_t		**exp, *p;

	while( 1 ){
		osMutex_take( TIMms_ExpireMutex, osMAX_TIME );

		if( TIMms_TopExpire != NULL ){
			exp = &TIMms_TopExpire;
			while( *exp != NULL ) {
				p = *exp;
				*exp = p->next_list;
				(p->expire_cb)( p );
			}
		}
	}
}

/**
 * @brief ���񔭉�TICK�l�Z�b�g.
 * @param[in] tick	������TICK�l.
 */
static void TIMms_setNextExpire( TICK16_t tick )
{
	/* �R���y�A�}�b�`���W�X�^��ύX����. */
	TIMER_ms_setCompReg( tick );

	TIMms_NextExpire = tick;
}

/**
 * @brief �~���b�^�C�}�@�\�̏����ݒ�API
 *
 * �~���b�^�C�}�R���g���[���̏����ݒ�y�у^�C�}���΃^�X�N�̐���
 */
void TIMms_initTimer( void )
{
	osTaskHandle_t	handle;
	int		retv;

printf("%s\n", __FUNCTION__);
	/* �^�C�}�N�� */
	TIMER_ms_init();

	/* �����Ǘ��f�[�^������ */
	//TIMms_Tick.tick_l = 0;
	TIMms_TopReq	 = NULL;
	TIMms_TailReq	 = NULL;
	TIMms_TopExpire	 = NULL;
	TIMms_NextExpire = (TICK16_t)-1;	//�ő�l�Z�b�g

	/* OS���\�[�X���� */
	TIMms_ExpireMutex = osMutex_create();

	/* ���΃R�[���o�b�N�p�^�X�N�𐶐�����. */
	retv = osTask_create( &TIMms_task, "TIMms", TIMms_STACKSZ/4,
						NULL, TIMmsTASK_PRI | portPRIVILEGE_BIT, &handle );
	configASSERT( retv == pdPASS );
}

/**
 * @brief �~���b�^�C�}�v��API
 *
 * �~���b�P�ʂ̃^�C�}�v����񋟂���API�֐��ł���.
 * ���΂̓R�[���o�b�N�ɂĒʒm����.
 *
 * @param[in]	time	   �^�C�}���ԁB�~���b�Ŏw�肷��.
 * @param[in]	expire_cb  ���Ύ��̃R�[���o�b�N�֐����w�肷��.
 * @param[out]	p_req	   �^�C�}�v���f�[�^�̈��n��.�����o�ϐ��ɂ͉����ݒ肷��K�v�͂Ȃ�.
 *						   �A�����΂��閘�A�f�[�^�̈��ێ����鎖.
 * @retval	handle	�^�C�}�v������������ƁA���̃^�C�}�n���h����Ԃ�.
 * @retval	NULL	�p�����[�^�G���[.
 * @pre		���� time��1ms�ȏ�ł��邱��.
 *			���� expire_cb��NULL�łȂ�����.
 *			���� p_req��NULL�łȂ�����.
 * @post	�^�C�}����͕Ԃ����n���h���Œ�~�\�ł���. �܂����Ύ��ɊY���n���h�����p�����[�^�Œʒm����.
 */
void *TIMms_reqTimer( int32_t time, void (*expire_cb)(void *handle), TIMms_t *p_req )
{
	uint16_t  tmp_tick;

	configASSERT(time > 0);
	configASSERT(expire_cb != NULL);
	configASSERT(p_req != NULL);

	p_req->old_tick	   = TIMER_ms_getTick32();
	p_req->remain_tick = MSEC_TO_TICK(time);
	p_req->expire_cb   = expire_cb;
	p_req->next_list   = NULL;
	p_req->feature	   = TIMms_FEATURE;

	osEnterCritical();
	if( TIMms_TailReq != NULL ){
		TIMms_TailReq->next_list = p_req;
		TIMms_TailReq = p_req;
	}else{
		TIMms_TailReq = p_req;
		TIMms_TopReq  = p_req;
	}
	osExitCritical();

	tmp_tick  = p_req->old_tick;
	tmp_tick += p_req->remain_tick;
	if( tmp_tick < TIMms_NextExpire ){
		TIMms_setNextExpire( (TICK16_t)tmp_tick );
	}
	return p_req;
}


/**
 * @brief �~���b�^�C�}���API
 *
 * �^�C�}�v���������API�֐��ł���.
 * API�̎g�����ɂ���Ă͎���Ɣ��΂�����Ⴄ�\��������̂ŌĂяo�����őΉ����邱��.
 *
 * @param[in]	handle	�^�C�}�v����������^�C�}�n���h��.
 * @retval	!NULL	�������������ƁA��������^�C�}�n���h����Ԃ�.
 * @retval	NULL	�p�����[�^�G���[.
 *					�܂��̓G���g������ĂȂ��i���ɔ��΂����j.
 * @pre		������handle��NULL�łȂ�����.
 */
void *TIMms_cancelTimer( TIMms_t *handle )
{
	TIMms_t		**pre, *p;

	osEnterCritical();
	pre = &TIMms_TopReq;
	p = TIMms_TopReq;
	while(p != NULL) {
		if( p == handle ) {
			if( TIMms_TailReq == handle ) {
				TIMms_TailReq = (*pre)->next_list;
			}
			*pre = p->next_list;
			break;
		}
		pre = &p->next_list;
		p = p->next_list;
	}
	osExitCritical();
	return p;
}

/**
 * @brief �~���b�^�C�}�����݃n���h������R�[������锭�Ώ���.
 * @param[in]	over =0/=1:�J�E���^Overflow.
 */
void TIMER_ms_expire( int over )
{
	portBASE_TYPE	dispatch;
	TIMms_t		**pre, *p;
	TIMms_t		**exp;
	TICK16_t	min_tick;
	uint32_t	temp_tick, now_tick;

	if( over ){
		// �v���p
		//ExtLED1_toggle();
	}

	dispatch  = pdFALSE;
	min_tick = (TICK16_t)-1;	//set MAX
	now_tick = TIMER_ms_getTick32();

	osEnterCritical();
	exp = &TIMms_TopExpire;
	while( *exp != NULL ) {
		*exp = (*exp)->next_list;
	}
	pre = &TIMms_TopReq;
	p = TIMms_TopReq;
	while(p != NULL) {
        temp_tick = now_tick - p->old_tick;

		p->remain_tick -= temp_tick;
		if( p->remain_tick <= 0 ) {
			 *pre = p->next_list;
			 p->next_list = NULL;
			 *exp = p;
			 exp = &p->next_list;
			 p = *pre;
		} else {
			if( p->remain_tick < min_tick ) {
				min_tick = (TICK16_t)p->remain_tick;
			}
			p->old_tick = now_tick;
			p = p->next_list;
		}
	}
	if( TIMms_TopReq == NULL ) {
		TIMms_TailReq = NULL;
	}
	TIMms_setNextExpire( min_tick );
	osExitCritical();

	if( TIMms_TopExpire != NULL ){
		/* �^�X�N���N������ */
		osMutex_giveISR( TIMms_ExpireMutex, &dispatch );
		portEND_SWITCHING_ISR( dispatch );
	}
}

/**
 * @} end of addtogroup
 */
