#include "osal.h"

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <inttypes.h>

#ifndef OSAL_LINUX_NO_WAIT
#define OSAL_LINUX_NO_WAIT 0
#endif // !OSAL_LINUX_NO_WAIT

#ifndef OSAL_LINUX_INF_WAIT
#define OSAL_LINUX_INF_WAIT (uint32_t)(-1)
#endif // !OSAL_LINUX_INF_WAIT

#define MAX_THREAD_NAME_LEN 16

const uint8_t gdPriorityLUT[OSAL_PRIO_MAX] = {
  1, /* 0 */
  25, /* 1 */
  50, /* 2 */
  75, /* 3 */
  99 /* 4 */  
};

static AddTimeout(struct timespec *rpCurrentTime, size_t rdTimeout)
{
    uint64_t dNumOfSec = (rdTimeout / MS_PER_SEC);
    uint64_t dNumOfNanoSec = ((rdTimeout - (dNumOfSec * MS_PER_SEC)) * NS_PER_SEC);
    dNumOfNanoSec = dNumOfNanoSec + rpCurrentTime->tv_nsec;

    if(dNumOfNanoSec >= NS_PER_SEC)
    {
        dNumOfNanoSec -= NS_PER_SEC;
        ++dNumOfSec;
    }
    rpCurrentTime->tv_nsec = dNumOfNanoSec;
    rpCurrentTime->tv_sec += dNumOfSec;
}

typedef struct
{
    char pName[MAX_THREAD_NAME_LEN + 1];
    pthread_t tThread;
    OSAL_TaskEntry_t entry;
    void *param;
} OSAL_TaskCall_t;

static void taskExit(void)
{
    // Terminate itself
    // vTaskDelete(NULL);
}

static void taskWrapper(void *rpContext)
{
    OSAL_TaskCall_t *taskCall = (OSAL_TaskCall_t*)(rpContext);

    taskCall->entry(taskCall->param);

    free(taskCall);
    taskExit();
}

size_t OSAL_ZeroTimeout(void)
{
    return OSAL_LINUX_NO_WAIT;
}

size_t OSAL_InfinityTimeout(void)
{
    return OSAL_LINUX_INF_WAIT;
}

OSAL_ErrorCode_t OSAL_Start(void)
{
    while(1)
    {
        OSAL_TaskSleep(10000);
    }

    return OSAL_OK;
}

bool OSAL_IsInISR(void)
{
    return false;
}

void OSAL_InterruptGlobalEnable(void)
{
    portENABLE_INTERRUPTS();
}

void OSAL_InterruptGlobalDisable(void)
{
    portDISABLE_INTERRUPTS();
}

//=====================================================================================================================
OSAL_ErrorCode_t OSAL_SemaphoreCreate(OSAL_Semaphore_t *rpSemaphore, size_t rdMaxCount, size_t rdInitCount)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    int32_t dRet;
    sem_t *pSem = (sem_t*)rpSemaphore;

    if(!pSem || (rdInitCount > rdMaxCount))
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else
    {
        if(!rdMaxCount)
        {
            rdMaxCount = OSAL_SEMAPHORE_DEFAULT_MAX;
        }

        pSem = (sem_t*)malloc(sizeof(sem_t));

        dRet = sem_init(pSem, 0, rdInitCount);

        if(!rpSemaphore || (dRet != 0))
        {
            tErr = OSAL_FAIL;
        }

        *rpSemaphore = (OSAL_Semaphore_t)pSem;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreWait(OSAL_Semaphore_t rtSemaphore, size_t rdTimeout)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;
    struct timespec tTimeout;
    clock_gettime(CLOCK_REALTIME, &tTimeout);
    AddTimeout(&tTimeout, rdTimeout);

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else
    {
        if(sem_timedwait(pSem, &tTimeout) != 0)
        {
            tErr = OSAL_TIMEOUT;
        }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphorePost(OSAL_Semaphore_t rtSemaphore)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else if(sem_post(pSem) != 0)
    {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreGetCount(OSAL_Semaphore_t rtSemaphore, size_t *rpCount)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;

    if(!pSem || !rpCount)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else if(sem_getvalue(pSem, (int*)rpCount) != 0)
    {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreDelete(OSAL_Semaphore_t rtSemaphore)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else if(sem_destroy(pSem) != 0)
    {
        tErr = OSAL_FAIL;
    }
    else
    {
        free(pSem);
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreCreate(OSAL_Semaphore_t *rpSemaphore)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    int32_t dRet;
    sem_t *pSem = (sem_t*)rpSemaphore;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else
    {
        pSem = (sem_t*)malloc(sizeof(sem_t));

        dRet = sem_init(pSem, 0, 0);

        if(!rpSemaphore || (dRet != 0))
        {
            tErr = OSAL_FAIL;
        }

        *rpSemaphore = (OSAL_Semaphore_t)pSem;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreWait(OSAL_Semaphore_t rtSemaphore, size_t rdTimeout)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;
    struct timespec tTimeout;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else
    {
        if(clock_gettime(CLOCK_REALTIME, &tTimeout) == -1)
        {
            tErr = OSAL_FAIL;
        }
        else
        {
            AddTimeout(&tTimeout, rdTimeout);

            int retVal = 0;
            while((retVal = sem_timedwait(pSem, &tTimeout)) == -1 && (errno ==EINTR))
            {
                continue;
            }

            if(retVal == -1)
            {
                if(errno == ETIMEDOUT)
                {
                    tErr = OSAL_TIMEOUT;
                }
                else
                {
                    tErr = OSAL_FAIL;
                }
            }
        }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphorePost(OSAL_Semaphore_t rtSemaphore)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;
    int32_t dSemVal;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else if(sem_getvalue(pSem, &dSemVal) || dSemVal || sem_post(pSem))
    {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreDelete(OSAL_Semaphore_t rtSemaphore)
{
    OSAL_ErrorCode_t tErr = OSAL_OK;
    sem_t *pSem = (sem_t*)rtSemaphore;

    if(!pSem)
    {
        tErr = OSAL_INVALID_PARAMS;
    }
    else if(sem_destroy(pSem) != 0)
    {
        tErr = OSAL_FAIL;
    }
    else
    {
        free(pSem);
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueCreate(OSAL_Queue_t *rpQueue, size_t rdQueueLength, size_t rdItemSize)
{
}

OSAL_ErrorCode_t OSAL_QueueLength(OSAL_Queue_t rtQueue, size_t *rpQueueLength)
{
}

OSAL_ErrorCode_t OSAL_QueueLevel(OSAL_Queue_t rtQueue, size_t *rpQueueLevel)
{
}

OSAL_ErrorCode_t OSAL_QueuePush(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout)
{
}

OSAL_ErrorCode_t OSAL_QueueGetFront(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout)
{
}

OSAL_ErrorCode_t OSAL_QueuePop(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout)
{
}

OSAL_ErrorCode_t OSAL_QueueFlush(OSAL_Queue_t rtQueue, size_t rdQueueItemSize)
{
}

OSAL_ErrorCode_t OSAL_QueueDelete(OSAL_Queue_t rtQueue)
{
}

OSAL_ErrorCode_t OSAL_TaskCreate(OSAL_Task_t *rpTask, OSAL_TaskEntry_t rtTaskEntry, const char *rpTaskName, uint32_t rdStackSize, void *rpTaskContext, uint8_t rdTaskPrio)
{
}

OSAL_ErrorCode_t OSAL_TaskDelete(OSAL_Task_t rtTask)
{
}

void OSAL_TaskSleep(uint32_t rdTimeoutMs)
{
}

OSAL_Priorities_t OSAL_TaskPrio(OSAL_Task_t rtTask)
{
}

OSAL_ErrorCode_t OSAL_TaskSetPrio(OSAL_Task_t rtTask, OSAL_Priorities_t rtPrio)
{
}

void OSAL_TaskExit(void)
{}

OSAL_ErrorCode_t OSAL_TaskSuspend(OSAL_Task_t rtTask)
{
}

OSAL_ErrorCode_t OSAL_TaskResume(OSAL_Task_t rtTask)
{
}

OSAL_ErrorCode_t OSAL_TaskSuspendAll()
{
}

OSAL_ErrorCode_t OSAL_TaskResumeAll()
{
}

OSAL_ErrorCode_t OSAL_TimerCreate(OSAL_Timer_t *rpTimer, OSAL_TimerType_t rtTimerType, uint32_t rdTimeoutMs, OSAL_TimerCallback_t rtTimerCb, void *rpTimerContext, const char* rpTimerName)
{
}

OSAL_ErrorCode_t OSAL_TimerStart(OSAL_Timer_t rtTimer, uint32_t rdTimeoutMs)
{
}

OSAL_ErrorCode_t OSAL_TimerStop(OSAL_Timer_t rtTimer)
{
}

bool OSAL_TimerIsRunning(OSAL_Timer_t rtTimer)
{
}

OSAL_ErrorCode_t OSAL_TimerDelete(OSAL_Timer_t rtTimer)
{
}