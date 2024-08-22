#include "logger.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static esp_log_level_t MapCoreLogLevelToPlatform(core_log_level_t rtLevel) {
    esp_log_level_t tPlatformLogLevel = ESP_LOG_INFO;

    switch (rtLevel) {
    case CORE_LOG_DEBUG:
        tPlatformLogLevel = ESP_LOG_DEBUG;
        break;
    case CORE_LOG_INFO:
        tPlatformLogLevel = ESP_LOG_INFO;
        break;
    case CORE_LOG_WARN:
        tPlatformLogLevel = ESP_LOG_WARN;
        break;
    case CORE_LOG_ERROR:
        tPlatformLogLevel = ESP_LOG_ERROR;
        break;
    case CORE_LOG_TRACE:
        tPlatformLogLevel = ESP_LOG_VERBOSE;
        break;
    default:
        break;
    }

    return tPlatformLogLevel;
}

void core_log_write(core_log_level_t level, const char *format, ...) {
    va_list list;
    va_start(list, format);

    const auto tPlatformLevel = MapCoreLogLevelToPlatform(level);
    esp_log_writev(tPlatformLevel, core_log_get_task_name(), format, list);

    va_end(list);
}

uint32_t core_log_get_timestamp(void) {
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) { return esp_log_early_timestamp(); }
    static uint32_t base = 0;
    if (base == 0 && xPortGetCoreID() == 0) { base = esp_log_early_timestamp(); }
    TickType_t tick_count = xPortInIsrContext() ? xTaskGetTickCountFromISR() : xTaskGetTickCount();
    return base + tick_count * (1000 / configTICK_RATE_HZ);
}

const char *core_log_get_task_name(void) {
    const TaskHandle_t tCurTaskHandle = xTaskGetCurrentTaskHandle();
    TaskStatus_t       tTaskDetails;

    vTaskGetInfo(tCurTaskHandle, &tTaskDetails, pdTRUE, eInvalid);
    return tTaskDetails.pcTaskName;
}