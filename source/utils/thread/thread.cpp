#include "thread.hpp"

CThread::CThread(ThreadFunc_t rtThreadCb, void *rpCtx, const char *rpName, uint8_t rdTaskPrio, size_t rdStackSize)
    : mtThreadCb{ rtThreadCb }, mpCtx{ rpCtx }, mpName{ rpName }, mdTaskPrio{ rdTaskPrio }, mdStackSize{ rdStackSize },
      mtTask{}, mbTaskCreated{ false }, mbRunTask{ false }, mtStartSignal{}, mtStoppedSignal{} {
    OSAL_SyncSemaphoreCreate(&mtStartSignal);
    OSAL_SyncSemaphoreCreate(&mtStoppedSignal);
    CreateThread();
}

CThread::~CThread() {}

void CThread::Start() {
    if (mtTask) {
        mbRunTask = true;
        OSAL_SyncSemaphorePost(mtStartSignal);
    }
}

void CThread::Stop() {
    if (mtTask) {
        if (mbTaskCreated) {
            if (!mbRunTask.exchange(false)) { OSAL_SyncSemaphorePost(mtStartSignal); }
        }

        OSAL_SyncSemaphoreWait(mtStoppedSignal, OSAL_INF_TIMEOUT);
    }

    OSAL_TaskDelete(mtTask);
    mtTask = (OSAL_Task_t) nullptr;
}

void CThread::Restart() {
    if (mtTask) { Stop(); }

    CreateThread();
    Start();
}

void CThread::Suspend() {
    if (mtTask) { OSAL_TaskSuspend(mtTask); }
}

void CThread::Resume() {
    if (mtTask) { OSAL_TaskResume(mtTask); }
}

bool CThread::IsRunning()
{
    return mbTaskCreated && mbRunTask;
}

void CThread::ThreadEntry(void *rpCtx) {
    CThread *pThis = (CThread *)(rpCtx);
    pThis->ThreadBase();
}

void CThread::ThreadBase() {
    OSAL_SyncSemaphoreWait(mtStartSignal, OSAL_INF_TIMEOUT);

    if (mbRunTask) { mtThreadCb(mpCtx); }

    mbRunTask     = false;
    mbTaskCreated = false;
    OSAL_SyncSemaphorePost(mtStoppedSignal);
    OSAL_TaskExit();
}

void CThread::SetPrio(uint8_t rdNewPrio) {
    mdTaskPrio = rdNewPrio;
    if (mtTask) { OSAL_TaskSetPrio(mtTask, (OSAL_Priorities_t)rdNewPrio); }
}

void CThread::CreateThread() {
    if (!mtTask) {
        mbTaskCreated = true;

        OSAL_TaskCreate(&mtTask, CThread::ThreadEntry, mpName, mdStackSize, this, mdTaskPrio);
    }
}
