/**
 * @file  TIMms.h
 * @brief �~���b�I�[�_�[�^�C�}�@�\.
 * 
 * �~���b�P�ʂŗv���\�ȃ\�t�g�E�F�A�^�C�}��APL�ɒ񋟂���.
 * 
 * @author      Teru
 * @date        2019/07/27
 * @version     Rev0.10
 * @copyright   2019 Emb-se.com.
 * 
 * @par �ύX����:
 * - Rev0.01: 2019/06/27: �V�K�쐬.
 * - Rev0.02: 2019/07/27: Doxygen�Ή�.
 * 
 * @copyright   2019 Emb-se.com.
 */
#ifndef _TIMms_H_
#define _TIMms_H_

#include "TIMER_ms.h"

/**
 * @page TIMms_title �^�C�}���C�u����
 *
 * @section TIMms_api API�ꗗ
 * | API��          | �@�\          |
 * |:-------------- |:------------- |
 * | TIMms_initTimer()  | �����ݒ�   |
 * | TIMms_reqTimer()   | �^�C�}�v�� |
 * | TIMms_cancelTimer()| �^�C�}��� |
 *
 * @section TIMms_usage �g�p���@
 * - �\�߈�x���� TIMms_initTimer() ���Ăяo���B
 */
/**
 * @addtogroup GROUP_TIMms �~���b�I�[�_�[�^�C�}�@�\.
 * @{
 */

/// �^�C�}�J�E���^���W�X�^�^(16bit�J�E���^)
//typedef uint16_t    TIMCNT_t;

/**
 * @brief �^�C�}�v���f�[�^�^.
 */
typedef struct TIMms_t {
    /// ���΂܂ł̎c��TICK
    int64_t     remain_tick;
    /// ���΃R�[���o�b�N�֐�.
    void   (*expire_cb)(void *hdl);
    /// �v���܂��͔��΃��X�g.
    struct TIMms_t *next_list;
    /// �v�������܂��͔��Ώ�������TICK�l.
    uint32_t    old_tick;
    /// �I�v�V�����@�\�R�[�h.
    uint16_t    feature;
}TIMms_t;

/**
 * @brief �I�v�V�����@�\�R�[�h.
 */
enum TIMms_OPTCD {
	TIMms_FEATURE	= 0x5000,		/**< �~���b�I�[�_�[�^�C�}�@�\�R�[�h */
	TIMms_STARTING	= 0x0100,		/**< �^�C�}�v�������� */
	TIMms_STOPING	= 0x0200,		/**< �^�C�}��~������ */
	TIMms_ONESHORT	= 0x0010,		/**< OneShot */
	TIMms_REPEAT	= 0x0020,		/**< ���s�[�g */
};

/// �~���b��tick�ϊ�. 1TICK=0.20msec
#define MSEC_TO_TICK(ms)		(int64_t)((int64_t)ms * 5) +1
/// tick���~���b�ϊ�. 1TICK=0.20msec
#define TICK_TO_MSEC(tick)		(int32_t)(tick / 5)

#define TIMms_getTick()     TIMER_ms_getTick32()

/**
 * @brief �����ݒ�API
 */
extern void TIMms_initTimer( void );

/**
 * @brief �^�C�}�v��API
 */
extern void *TIMms_reqTimer( int32_t time, void (*expire_cb)(void *handle), TIMms_t *p_req );

/**
 * @brief �^�C�}���API
 */
extern void *TIMms_cancelTimer( TIMms_t *handle );

/**
 * @brief �^�C�}�����݃n���h������R�[������锭�Ώ���
 */
extern void TIMms_expire( int over );

/**
 * @} end of addtogroup
 */
#endif //_TIMms_H_
