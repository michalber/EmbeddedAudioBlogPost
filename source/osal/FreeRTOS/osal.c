#include "osal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#ifndef OSAL_FREE_RT_NO_WAIT
#define OSAL_FREE_RT_NO_WAIT 0
#endif// !OSAL_FREE_RT_NO_WAIT

#ifndef OSAL_FREE_RT_INF_WAIT
#define OSAL_FREE_RT_INF_WAIT portMAX_DELAY
#endif// !OSAL_FREE_RT_INF_WAIT

static const char *OSAL_TASK_STATE_STR[] = { "Running", "Ready", "Blocked", "Suspended", "Deleted", "Invalid" };

const uint8_t gdPriorityLUT[OSAL_PRIO_MAX] = {
    0,  /* 0 */
    4,  /* 1 */
    9,  /* 2 */
    14, /* 3 */
    19  /* 4 */
};

typedef struct {
    OSAL_TaskEntry_t entry;
    void            *param;
} OSAL_TaskCall_t;

static void taskExit(void) {
    // Terminate itself
    vTaskDelete(NULL);
}

static void taskWrapper(void *rpContext) {
    OSAL_TaskCall_t *taskCall = (OSAL_TaskCall_t *)(rpContext);

    taskCall->entry(taskCall->param);

    free(taskCall);
    taskExit();
}

size_t OSAL_ZeroTimeout(void) { return OSAL_FREE_RT_NO_WAIT; }

size_t OSAL_InfinityTimeout(void) { return OSAL_FREE_RT_INF_WAIT; }

OSAL_ErrorCode_t OSAL_Start(void) {
    vTaskStartScheduler();

    return OSAL_OK;
}

bool OSAL_IsInISR(void) {
    BaseType_t res = xPortInIsrContext();
    return (res == pdTRUE) ? true : false;
}

void OSAL_InterruptGlobalEnable(void) { portENABLE_INTERRUPTS(); }

void OSAL_InterruptGlobalDisable(void) { portDISABLE_INTERRUPTS(); }

//=====================================================================================================================
OSAL_ErrorCode_t OSAL_SemaphoreCreate(OSAL_Semaphore_t *rpSemaphore, size_t rdMaxCount, size_t rdInitCount) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rpSemaphore || (rdInitCount > rdMaxCount)) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        if (!rdMaxCount) { rdMaxCount = OSAL_SEMAPHORE_DEFAULT_MAX; }

        *rpSemaphore = (size_t)xSemaphoreCreateCounting(rdMaxCount, rdInitCount);

        if (!rpSemaphore) { tErr = OSAL_FAIL; }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreWait(OSAL_Semaphore_t rtSemaphore, size_t rdTimeout) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xSemaphoreTake((QueueHandle_t)rtSemaphore, (TickType_t)(rdTimeout / portTICK_PERIOD_MS)) != pdTRUE) {
        tErr = OSAL_TIMEOUT;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphorePost(OSAL_Semaphore_t rtSemaphore) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xSemaphoreGive((QueueHandle_t)rtSemaphore) != pdTRUE) {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreGetCount(OSAL_Semaphore_t rtSemaphore, size_t *rpCount) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore || !rpCount) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        *rpCount = uxSemaphoreGetCount((QueueHandle_t)rtSemaphore);
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SemaphoreDelete(OSAL_Semaphore_t rtSemaphore) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        vSemaphoreDelete((QueueHandle_t)rtSemaphore);
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreCreate(OSAL_Semaphore_t *rpSemaphore) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rpSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        *rpSemaphore = (size_t)xSemaphoreCreateBinary();

        if (!rpSemaphore) { tErr = OSAL_FAIL; }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreWait(OSAL_Semaphore_t rtSemaphore, size_t rdTimeout) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xSemaphoreTake((QueueHandle_t)rtSemaphore, (TickType_t)(rdTimeout / portTICK_PERIOD_MS)) != pdTRUE) {
        tErr = OSAL_TIMEOUT;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphorePost(OSAL_Semaphore_t rtSemaphore) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xSemaphoreGive((QueueHandle_t)rtSemaphore) != pdTRUE) {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreDelete(OSAL_Semaphore_t rtSemaphore) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtSemaphore) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        vSemaphoreDelete((QueueHandle_t)rtSemaphore);
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueCreate(OSAL_Queue_t *rpQueue, size_t rdQueueLength, size_t rdItemSize) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rpQueue || !rdQueueLength || !rdItemSize) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        *rpQueue = (size_t)xQueueCreate(rdQueueLength, rdItemSize);

        if (!rpQueue) { tErr = OSAL_FAIL; }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueLength(OSAL_Queue_t rtQueue, size_t *rpQueueLength) { return OSAL_INVALID; }

OSAL_ErrorCode_t OSAL_QueueLevel(OSAL_Queue_t rtQueue, size_t *rpQueueLevel) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rpQueueLevel || !rpQueueLevel) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        *rpQueueLevel = (size_t)uxQueueMessagesWaiting((QueueHandle_t)rtQueue);
        tErr          = OSAL_OK;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueuePush(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtQueue || !rpItem) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xQueueSend((void *)rtQueue, rpItem, rdTimeout) != pdTRUE) {
        tErr = OSAL_TIMEOUT;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueGetFront(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtQueue || !rpItem) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xQueuePeek((void *)rtQueue, rpItem, rdTimeout) != pdTRUE) {
        tErr = OSAL_TIMEOUT;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueuePop(OSAL_Queue_t rtQueue, void *rpItem, size_t rdTimeout) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtQueue || !rpItem) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xQueueReceive((void *)rtQueue, rpItem, rdTimeout) != pdTRUE) {
        tErr = OSAL_TIMEOUT;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueFlush(OSAL_Queue_t rtQueue, size_t rdQueueItemSize) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtQueue || !rdQueueItemSize) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        xQueueReset((void *)rtQueue);
        tErr = OSAL_OK;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_QueueDelete(OSAL_Queue_t rtQueue) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtQueue) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        vQueueDelete((QueueHandle_t)rtQueue);
        tErr = OSAL_OK;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_TaskCreate(OSAL_Task_t     *rpTask,
                                 OSAL_TaskEntry_t rtTaskEntry,
                                 const char      *rpTaskName,
                                 uint32_t         rdStackSize,
                                 void            *rpTaskContext,
                                 uint8_t          rdTaskPrio) {
    OSAL_ErrorCode_t tErr = OSAL_OK;
    BaseType_t       tRet;

    if (!rpTask || !rtTaskEntry || !rpTaskName) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        OSAL_TaskCall_t *taskCall = (OSAL_TaskCall_t *)malloc(sizeof(OSAL_TaskCall_t));
        taskCall->entry           = rtTaskEntry;
        taskCall->param           = rpTaskContext;

        tRet = xTaskCreate(taskWrapper,
                           rpTaskName,
                           (rdStackSize + sizeof(portSTACK_TYPE) - 1 / sizeof(portSTACK_TYPE)),
                           taskCall,
                           OSAL_PriorityLUTConv(rdTaskPrio),
                           (void *)rpTask);

        if (tRet != pdPASS) { tErr = OSAL_FAIL; }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_TaskDelete(OSAL_Task_t rtTask) { return OSAL_OK; }

void OSAL_TaskSleep(uint32_t rdTimeoutMs) {
    if (rdTimeoutMs) {
        TickType_t tDelay = rdTimeoutMs / portTICK_PERIOD_MS;
        vTaskDelay(tDelay);
    }
}

OSAL_Priorities_t OSAL_TaskPrio(OSAL_Task_t rtTask) {
    uint8_t basePrio = uxTaskPriorityGet((TaskHandle_t)rtTask);

    int8_t foundPrioIdx = -1;
    for (uint8_t idx = 0; idx < OSAL_PRIO_MAX; ++idx) {
        if (gdPriorityLUT[idx] == basePrio) { foundPrioIdx = idx; }
    }

    return foundPrioIdx;
}

OSAL_ErrorCode_t OSAL_TaskSetPrio(OSAL_Task_t rtTask, OSAL_Priorities_t rtPrio) {
    if (rtPrio <= OSAL_PRIO_MAX) {
        vTaskPrioritySet((TaskHandle_t)rtTask, (UBaseType_t)(gdPriorityLUT[OSAL_PriorityLUTConv(rtPrio)]));
    }
    return OSAL_OK;
}

void OSAL_TaskExit(void) {}

OSAL_ErrorCode_t OSAL_TaskSuspend(OSAL_Task_t rtTask) {
    vTaskSuspend((TaskHandle_t)rtTask);
    return OSAL_OK;
}

OSAL_ErrorCode_t OSAL_TaskResume(OSAL_Task_t rtTask) {
    vTaskResume((TaskHandle_t)rtTask);
    return OSAL_OK;
}

OSAL_ErrorCode_t OSAL_TaskSuspendAll() {
    vTaskSuspendAll();
    return OSAL_OK;
}

OSAL_ErrorCode_t OSAL_TaskResumeAll() {
    if (xTaskResumeAll() == pdTRUE) { return OSAL_OK; }

    return OSAL_FAIL;
}

OSAL_ErrorCode_t OSAL_TimerCreate(OSAL_Timer_t        *rpTimer,
                                  OSAL_TimerType_t     rtTimerType,
                                  uint32_t             rdTimeoutMs,
                                  OSAL_TimerCallback_t rtTimerCb,
                                  void                *rpTimerContext,
                                  const char          *rpTimerName) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rpTimer || !rtTimerCb || !rpTimerContext || !rpTimerName) {
        tErr = OSAL_INVALID_PARAMS;
    } else {
        *rpTimer = (size_t)xTimerCreate(rpTimerName,
                                        (TickType_t)(rdTimeoutMs / portTICK_PERIOD_MS),
                                        (UBaseType_t)(rtTimerType == OSAL_TIMER_PERIODIC ? pdTRUE : pdFALSE),
                                        (void *)0,
                                        rtTimerCb);

        if (!rpTimer) { tErr = OSAL_FAIL; }
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_TimerStart(OSAL_Timer_t rtTimer, uint32_t rdTimeoutMs) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtTimer || !rdTimeoutMs) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xTimerStart((TimerHandle_t)rtTimer, rdTimeoutMs) != pdTRUE) {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_TimerStop(OSAL_Timer_t rtTimer) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtTimer) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xTimerStop((TimerHandle_t)rtTimer, OSAL_InfinityTimeout()) != pdTRUE) {
        tErr = OSAL_FAIL;
    }

    return tErr;
}

bool OSAL_TimerIsRunning(OSAL_Timer_t rtTimer) {
    bool tErr = true;

    if (!rtTimer) {
        tErr = false;
    } else if (xTimerIsTimerActive((TimerHandle_t)rtTimer) != pdTRUE) {
        tErr = false;
    }

    return tErr;
}

OSAL_ErrorCode_t OSAL_TimerDelete(OSAL_Timer_t rtTimer) {
    OSAL_ErrorCode_t tErr = OSAL_OK;

    if (!rtTimer) {
        tErr = OSAL_INVALID_PARAMS;
    } else if (xTimerDelete((TimerHandle_t)rtTimer, OSAL_INF_TIMEOUT) != pdTRUE) {
        tErr = OSAL_FAIL;
    }

    return tErr;
}