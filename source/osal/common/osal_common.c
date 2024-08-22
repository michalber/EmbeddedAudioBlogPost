#include "osal.h"

const uint8_t gdPrioritiesLUT[OSAL_PRIO_MAX] = {
    0, /* OSAL_PRIO_LOW */
    1, /* OSAL_PRIO_MID */
    2, /* OSAL_PRIO_HIGH */
    3, /* OSAL_PRIO_REALTIME */
    4  /* OSAL_PRIO_HIGHEST */
};

static uint8_t OSAL_GetPrioLUTVal(OSAL_Priorities_t rePrio) { return gdPrioritiesLUT[rePrio]; }

uint8_t OSAL_PriorityLUTConv(uint8_t rdOsalPrio) { return gdPrioritiesLUT[rdOsalPrio]; }

OSAL_Priorities_t OSAL_PriorityLUTUnconv(uint8_t rdOsalPrio) {
    int8_t foundPrioIdx = -1;
    for (uint8_t idx = 0; idx < OSAL_PRIO_MAX; ++idx) {
        if (gdPrioritiesLUT[idx] == rdOsalPrio) { foundPrioIdx = idx; }
    }

    return foundPrioIdx > 0 ? foundPrioIdx : OSAL_PRIO_MID;
}

OSAL_ErrorCode_t OSAL_SemaphoreClear(OSAL_Semaphore_t rtSemaphore) {
    OSAL_ErrorCode_t tErr;

    do { tErr = OSAL_SemaphoreWait(rtSemaphore, OSAL_ZERO_TIMEOUT); } while (tErr == OSAL_OK);

    return tErr == OSAL_TIMEOUT ? OSAL_OK : tErr;
}

OSAL_ErrorCode_t OSAL_SyncSemaphoreClear(OSAL_Semaphore_t rtSemaphore) {
    const OSAL_ErrorCode_t tErr = OSAL_SemaphoreWait(rtSemaphore, OSAL_ZERO_TIMEOUT);

    return tErr == OSAL_TIMEOUT ? OSAL_OK : tErr;
}