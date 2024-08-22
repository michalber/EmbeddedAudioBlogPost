#ifndef __TIMER_IMPL_HPP__
#define __TIMER_IMPL_HPP__

#include "timer_int.hpp"
#include <string_view>
#include "esp_timer.h"

class CTimer : public ITimer {
  public:
    CTimer(TimerEvtCb rpEvtCb, TimerMode rtMode, uint32_t rdTimeoutMs, void *rpCtx, const std::string_view &rtName);
    ~CTimer();

    bool Start() override;
    bool Stop() override;
    bool Restart() override;

  private:
    TimerEvtCb             mEvtCb;
    TimerMode              mMode;
    uint32_t               mTimeoutMs;
    void                  *mCtx;
    const std::string_view mName;
    esp_timer_handle_t     mTimerHandle;
};

#endif// !__TIMER_IMPL_HPP__
