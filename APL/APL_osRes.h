#ifndef _APL_osRes_h_
#define _APL_osRes_h_

enum {
	IDLE_TASKID = 0,
	TIMms_TASKID,
	LOG_TASKID,
	shell_TASKID,
	APL_INITTASKID,
	SurveyA_TASKID,
	SurveyB_TASKID,
	MAX_TASKNUM,
};
enum {
	LOG_QUEID = 0,
	MAX_QUENUM,
};
enum {
	TIMms_MUTEXID = 0,
	UART_RXMUTEXID,
	APL_INITMUTEXID,
	SurveyA_MUTEXID,
	SurveyB_MUTEXID,
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
#define	TIMms_STACKSZ		384
#define	LOG_STACKSZ			384
#define	shell_STACKSZ		384
#define	APLinit_STACKSZ		384
#define	SurveyA_STACKSZ		384
#define	SurveyB_STACKSZ		384
#define TOTAL_STACKSZ		( osIDLE_STACKSZ \
							+ TIMms_STACKSZ \
							+ LOG_STACKSZ \
							+ shell_STACKSZ \
							+ APLinit_STACKSZ \
							+ SurveyA_STACKSZ \
							+ SurveyB_STACKSZ )

// キューのコンテンツ・メモリサイズ(コンテンツサイズ*個数)
//#define	osTMRSVC_QUESZ		( 12*10 +8 )
#define	LOG_QUESZ			( 44*10 +8 )
#define TOTAL_QUESZ			( LOG_QUESZ )

// OSヒープサイズ
#define osHEAP_SZ			( TOTAL_MNGSZ			\
							+ TOTAL_STACKSZ 		\
							+ TOTAL_QUESZ)

#define TIMmsTASK_PRI		(configMAX_PRIORITIES -1)
#define LOGTASK_PRI			(configMAX_PRIORITIES -2)
#define shellTASK_PRI		(configMAX_PRIORITIES -4)
#define APLinitTASK_PRI		(configMAX_PRIORITIES -4)
#define SurveyA_PRI			(configMAX_PRIORITIES -3)
#define SurveyB_PRI			(configMAX_PRIORITIES -3)


#endif // _APL_osRes.h_
