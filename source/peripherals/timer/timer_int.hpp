#ifndef __TIMERI_HPP__
#define __TIMERI_HPP__

#include <cstdint>
#include <functional>

namespace periph::timer {
int64_t GetCurrentTime();
}

typedef void (*TimerEvtCb)(void *rpCtx);

enum class TimerMode { kOneShot, kPeriodic, kLast };

class CMeasureTimer {
  public:
    using CbFunction = std::function<void(const int64_t &, const int64_t &, const int64_t &)>;
    CMeasureTimer(CbFunction &&cb) : mCallback{ cb } { mStartTime = periph::timer::GetCurrentTime(); }

    ~CMeasureTimer() {
        mEndTime  = periph::timer::GetCurrentTime();
        mDuration = mEndTime - mStartTime;
        mCallback(mStartTime, mEndTime, mDuration);
    }

  private:
    CbFunction mCallback;
    int64_t    mStartTime;
    int64_t    mEndTime{0};
    int64_t    mDuration{0};
};

class ITimer {
  public:
    ITimer()          = default;
    virtual ~ITimer() = default;

    virtual bool Start()   = 0;
    virtual bool Stop()    = 0;
    virtual bool Restart() = 0;

  private:
};

#endif// !__TIMERI_HPP__
