#include "timer.hpp"

#include "time.hpp"

namespace periph::timer
{
  int64_t GetCurrentTime()
  {
    return esp_timer_get_time();
  }
}


CTimer::CTimer(TimerEvtCb rpEvtCb, TimerMode rtMode, uint32_t rdTimeoutMs, void *rpCtx, const std::string_view &rtName)
    : mEvtCb{ rpEvtCb }, mMode{ rtMode }, mTimeoutMs{ rdTimeoutMs }, mCtx{ rpCtx }, mName{ rtName } {
    esp_timer_create_args_t tTimerArgs;
    tTimerArgs.callback        = mEvtCb;
    tTimerArgs.arg             = mCtx;
    tTimerArgs.name            = mName.data();
    tTimerArgs.dispatch_method = ESP_TIMER_TASK;
    esp_err_t tErr             = esp_timer_create(&tTimerArgs, &mTimerHandle);
    // CORE_ASSERT(tErr == ESP_OK);
}

CTimer::~CTimer() {
    Stop();
    esp_timer_delete(mTimerHandle);
}

bool CTimer::Start() {
    esp_err_t tErr = ESP_OK;

    switch (mMode) {
    case TimerMode::kOneShot:
        tErr = esp_timer_start_once(mTimerHandle, MSEC_TO_USEC(mTimeoutMs));
        break;
    case TimerMode::kPeriodic:
        tErr = esp_timer_start_periodic(mTimerHandle, MSEC_TO_USEC(mTimeoutMs));
        break;
    default:
        break;
    }

    return tErr == ESP_OK;
}

bool CTimer::Stop() {
    const esp_err_t tErr = esp_timer_stop(mTimerHandle);
    return tErr == ESP_OK;
}

bool CTimer::Restart() {
    const esp_err_t tErr = esp_timer_restart(mTimerHandle, mTimeoutMs * 1000);
    return tErr == ESP_OK;
}