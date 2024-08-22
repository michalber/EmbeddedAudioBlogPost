#ifndef __UTILS_THREAD_HPP__
#define __UTILS_THREAD_HPP__

#include "osal.h"
#include "types.h"

#include <atomic>

constexpr auto kTaskDefaultStackSize = 0x500;
constexpr auto kTaskDefaultPrio      = OSAL_PRIO_MID;

typedef void (*ThreadFunc_t)(void *rpCtx);

class CThread {
  public:
    CThread(ThreadFunc_t rtThreadCb,
            void        *rpCtx,
            const char  *rpName,
            uint8_t      rdTaskPrio  = kTaskDefaultPrio,
            size_t       rdStackSize = kTaskDefaultStackSize);

    ~CThread();

    void Start();
    void Stop();
    void Restart();
    void Suspend();
    void Resume();
    bool IsRunning();

    static void ThreadEntry(void *rpCtx);
    void        ThreadBase();

    void SetPrio(uint8_t rdNewPrio);


  private:
    void CreateThread();

  private:
    ThreadFunc_t         mtThreadCb;
    void                *mpCtx;
    const char          *mpName;
    uint8_t              mdTaskPrio;
    size_t               mdStackSize;
    OSAL_Task_t          mtTask;
    std::atomic_bool     mbTaskCreated;
    std::atomic_bool     mbRunTask;
    OSAL_SyncSemaphore_t mtStartSignal;
    OSAL_SyncSemaphore_t mtStoppedSignal;

    NO_COPY_OR_ASSIGN(CThread);
};


#endif// !__UTILS_THREAD_HPP__