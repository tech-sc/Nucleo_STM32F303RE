#include "os.h"

//#ifdef USE_FULL_ASSERT
#include "stm32_assert.h"
/******************************************************************************
 * @brief   assert_param() fail function
 * @param   (I)file		pointer to the source file name
 * @param   (I)line		line source number
 * @retval  None
 */
void assert_failed(const char* file, int line)
{
	for( ;; );
}
//#endif /* USE_FULL_ASSERT */

#if( configUSE_TICK_HOOK ==1 )
/******************************************************************************
 * @brief   Tick hook function
 * @param   None
 * @retval  None
 * @note    Maybe this function will be called from the Tick handler.
 */
void vApplicationTickHook( void )
{
}
#endif

#if( configUSE_IDLE_HOOK == 1 )
/******************************************************************************
 * @brief   Idle task hook function
 * @param   None
 * @retval  None
 * @note    This fuction will only be called if configUSE_IDLE_HOOK is set
 *          to 1 in FreeRTOSConfig.h.
 *          It will be called from the idle task.
 */
void vApplicationIdleHook( void )
{
//	__WFI();
}
#endif /* configUSE_IDLE_HOOK */

#if( configCHECK_FOR_STACK_OVERFLOW != 0 )
/******************************************************************************
 * @brief   Run time stack overflow hook function
 * @param   (I)pxTask
 * @param   (I)pcTaskName
 * @retval  None
 * @note    Run time stack overflow checking is performed if
 *          configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.
 *          This hook function is called if a stack overflow is detected.
 */
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
#endif /* configCHECK_FOR_STACK_OVERFLOW */

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
/******************************************************************************
 * @brief   pvPortMalloc() fail hook function
 * @param   None
 * @retval  None
 * @note    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.
 *          It is a hook function that will get called if a call to pvPortMalloc() fails.
 *          If heap_1.c or heap_2.c are used, then the size of the heap
 *          available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
 *          FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
 *          to query the size of free heap space that remains.
 */
void vApplicationMallocFailedHook( void )
{
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif /* configUSE_MALLOC_FAILED_HOOK */
