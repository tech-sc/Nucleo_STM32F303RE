#ifndef _APL_osRes_h_
#define _APL_osRes_h_

enum {
	IDLE_TASKID = 0,
	TMRSVC_TASKID,
	SHELL_TASKID,
	TIMms_TASKID,
	LOG_TASKID,
	APL_INITTASKID,
	AplTemp_TASKID,
	MAX_TASKNUM,
};
enum {
	TMRSVC_QUEID = 0,
	LOG_QUEID,
	AplTemp_QUEID,
	MAX_QUENUM,
};
enum {
	UART_RXMUTEXID = 0,
	TIMms_MUTEXID,
	APL_INITMUTEXID,
	AplTemp_MUTEXID,
	MAX_MUTEXNUM,
};

// OSリソースの管理に使用されるメモリサイズ
#define	osRESOURCEMNG_SZ	( 80+8+8 )
#define	osTASKMNG_SZ		( MAX_TASKNUM  * osRESOURCEMNG_SZ )
#define	osQUEMNG_SZ			( MAX_QUENUM   * osRESOURCEMNG_SZ )
#define	osMUTEXMNG_SZ		( MAX_MUTEXNUM * osRESOURCEMNG_SZ )
#define	TOTAL_MNGSZ			( osTASKMNG_SZ			\
							+ osQUEMNG_SZ			\
							+ osMUTEXMNG_SZ )

// タスクのスタック・メモリサイズ
#define	osIDLE_STACKSZ		256
#define	osTMRSVC_STACKSZ	256
#define	shell_STACKSZ		384
#define	TIMms_STACKSZ		256
#define	LOG_STACKSZ			384
#define	APLinit_STACKSZ		256
#define	AplTemp_STACKSZ		256
#define	SwCtrl_STACKSZ		256
#define TOTAL_STACKSZ		( osIDLE_STACKSZ \
							+ osTMRSVC_STACKSZ \
							+ shell_STACKSZ \
							+ TIMms_STACKSZ \
							+ LOG_STACKSZ \
							+ APLinit_STACKSZ \
							+ AplTemp_STACKSZ \
							+ SwCtrl_STACKSZ )

// キューのコンテンツ・メモリサイズ(コンテンツサイズ*個数)
#define	osTMRSVC_QUESZ		( 12*10 +8 )
#define	LOG_QUESZ			( 44*10 +8 )
#define	AplTemp_QUESZ		( 16*3  +8 )
#define TOTAL_QUESZ			( osTMRSVC_QUESZ \
							+ LOG_QUESZ \
							+ AplTemp_QUESZ )

// OSヒープサイズ
#define osHEAP_SZ			( TOTAL_MNGSZ			\
							+ TOTAL_STACKSZ 		\
							+ TOTAL_QUESZ)

#define TIMmsTASK_PRI		(configMAX_PRIORITIES -1)
#define shellTASK_PRI		(configMAX_PRIORITIES -3)
#define LOGTASK_PRI			(configMAX_PRIORITIES -4)
#define APLinitTASK_PRI		(configMAX_PRIORITIES -4)
#define AplTempTASK_PRI		(configMAX_PRIORITIES -4)
#define SwCtrlTASK_PRI		(configMAX_PRIORITIES -3)


#endif // _APL_osRes.h_
