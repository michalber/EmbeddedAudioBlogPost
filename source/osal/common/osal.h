#ifndef __OSAL_H__
#define __OSAL_H__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define OSAL_ZERO_TIMEOUT OSAL_ZeroTimeout()
#define OSAL_INF_TIMEOUT OSAL_InfinityTimeout()
#define OSAL_SEMAPHORE_DEFAULT_MAX 100

typedef size_t OSAL_Semaphore_t;
typedef size_t OSAL_SyncSemaphore_t;
typedef size_t OSAL_Task_t;
typedef size_t OSAL_Queue_t;

typedef enum
{
    OSAL_OK = 0,
    OSAL_FAIL,
    OSAL_INVALID_PARAMS,
    OSAL_TIMEOUT,
    OSAL_INVALID
} OSAL_ErrorCode_t;

typedef enum
{
    OSAL_PRIO_HIGHEST = 0,
    OSAL_PRIO_REALTIME,
    OSAL_PRIO_HIGH,
    OSAL_PRIO_MID,
    OSAL_PRIO_LOW,

    OSAL_PRIO_MAX
} OSAL_Priorities_t;

typedef enum
{
    OSAL_TIMER_ONE_SHOT = 0,
    OSAL_TIMER_PERIODIC
} OSAL_TimerType_t;

typedef void (*OSAL_TimerCallback_t)(void *rpContext);
typedef void* OSAL_Timer_t;

typedef void (*OSAL_TaskEntry_t)(void *rpContext);

//=====================================================================================================================
OSAL_ErrorCode_t OSAL_Start(void);
bool OSAL_IsInISR(void);
size_t OSAL_ZeroTimeout(void);
size_t OSAL_InfinityTimeout(void);
void OSAL_InterruptGlobalEnable(void);
void OSAL_InterruptGlobalDisable(void);
uint8_t OSAL_PriorityLUTConv(uint8_t rdOsalPrio);
OSAL_Priorities_t OSAL_PriorityLUTUnconv(uint8_t rdOsalPrio);

OSAL_ErrorCode_t OSAL_SemaphoreCreate(OSAL_Semaphore_t *rpSemaphore, size_t rdMaxCount, size_t rdInitCount);
OSAL_ErrorCode_t OSAL_SemaphoreWait(OSAL_Semaphore_t rtSemaphore, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_SemaphorePost(OSAL_Semaphore_t rtSemaphore);
OSAL_ErrorCode_t OSAL_SemaphoreGetCount(OSAL_Semaphore_t rtSemaphore, size_t *rpCount);
OSAL_ErrorCode_t OSAL_SemaphoreClear(OSAL_Semaphore_t rtSemaphore);
OSAL_ErrorCode_t OSAL_SemaphoreDelete(OSAL_Semaphore_t rtSemaphore);

OSAL_ErrorCode_t OSAL_SyncSemaphoreCreate(OSAL_SyncSemaphore_t *rpSemaphore);
OSAL_ErrorCode_t OSAL_SyncSemaphoreWait(OSAL_SyncSemaphore_t rtSemaphore, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_SyncSemaphorePost(OSAL_SyncSemaphore_t rtSemaphore);
OSAL_ErrorCode_t OSAL_SyncSemaphoreClear(OSAL_SyncSemaphore_t rtSemaphore);
OSAL_ErrorCode_t OSAL_SyncSemaphoreDelete(OSAL_SyncSemaphore_t rtSemaphore);

OSAL_ErrorCode_t OSAL_QueueCreate(OSAL_Queue_t *rpQueue, size_t rdQueueLength, size_t rdItemSize);
OSAL_ErrorCode_t OSAL_QueueLength(OSAL_Queue_t rtQueue, size_t *rpQueueLength);
OSAL_ErrorCode_t OSAL_QueueLevel(OSAL_Queue_t rtQueue, size_t *rpQueueLevel);
OSAL_ErrorCode_t OSAL_QueuePush(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_QueueGetFront(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_QueueGetBack(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_QueuePop(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout);
OSAL_ErrorCode_t OSAL_QueueFlush(OSAL_Queue_t rtQueue, size_t rdQueueItemSize);
OSAL_ErrorCode_t OSAL_QueueDelete(OSAL_Queue_t rtQueue);

OSAL_ErrorCode_t OSAL_TaskCreate(OSAL_Task_t *rpTask, OSAL_TaskEntry_t rtTaskEntry, const char *rpTaskName, uint32_t rdStackSize, void *rpTaskContext, uint8_t rdTaskPrio);
OSAL_ErrorCode_t OSAL_TaskDelete(OSAL_Task_t rtTask);
void OSAL_TaskSleep(uint32_t rdTimeoutMs);
OSAL_Priorities_t OSAL_TaskPrio(OSAL_Task_t rtTask);
OSAL_ErrorCode_t OSAL_TaskSetPrio(OSAL_Task_t rtTask, OSAL_Priorities_t rtPrio);
void OSAL_TaskExit(void);
OSAL_ErrorCode_t OSAL_TaskSuspend(OSAL_Task_t rtTask);
OSAL_ErrorCode_t OSAL_TaskResume(OSAL_Task_t rtTask);

OSAL_ErrorCode_t OSAL_TaskSuspendAll();
OSAL_ErrorCode_t OSAL_TaskResumeAll();

OSAL_ErrorCode_t OSAL_TimerCreate(OSAL_Timer_t *rpTimer, OSAL_TimerType_t rtTimerType, uint32_t rdTimeoutMs, OSAL_TimerCallback_t rtTimerCb, void *rpTimerContext, const char* rpTimerName);
OSAL_ErrorCode_t OSAL_TimerStart(OSAL_Timer_t rtTimer, uint32_t rdTimeoutMs);
OSAL_ErrorCode_t OSAL_TimerStop(OSAL_Timer_t rtTimer);
bool OSAL_TimerIsRunning(OSAL_Timer_t rtTimer);
OSAL_ErrorCode_t OSAL_TimerDelete(OSAL_Timer_t rtTimer);

#ifdef __cplusplus
}
#endif

#endif // !__OSAL_H__