#ifndef _ESP32_LOGGER_
#define _ESP32_LOGGER_

#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>

#define CORE_LOG_FORMAT(letter, format)  #letter " (%" PRIu32 ") %s: " format "\n"

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define CORE_LOG_ERROR(format, ...) core_log_write(CORE_LOG_ERROR, CORE_LOG_FORMAT(E, format), core_log_get_timestamp(), core_log_get_task_name(), __VA_OPT__(,) __VA_ARGS__)
#define CORE_LOG_WARN(format, ...) core_log_write(CORE_LOG_WARN, CORE_LOG_FORMAT(W, format), core_log_get_timestamp(), core_log_get_task_name(), __VA_OPT__(,) __VA_ARGS__)
#define CORE_LOG_INFO(format, ...) core_log_write(CORE_LOG_INFO, CORE_LOG_FORMAT(I, format), core_log_get_timestamp(), core_log_get_task_name(), __VA_OPT__(,) __VA_ARGS__)
#define CORE_LOG_DEBUG(format, ...) core_log_write(CORE_LOG_DEBUG, CORE_LOG_FORMAT(D, format), core_log_get_timestamp(), core_log_get_task_name(), __VA_OPT__(,) __VA_ARGS__)
#define CORE_LOG_TRACE(format, ...) core_log_write(CORE_LOG_TRACE, CORE_LOG_FORMAT(T, format), core_log_get_timestamp(), core_log_get_task_name(), __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define CORE_LOG_ERROR(format, ...) core_log_write(CORE_LOG_ERROR, CORE_LOG_FORMAT(E, format), core_log_get_timestamp(), core_log_get_task_name(), ##__VA_ARGS__)
#define CORE_LOG_WARN(format, ...) core_log_write(CORE_LOG_WARN, CORE_LOG_FORMAT(W, format), core_log_get_timestamp(), core_log_get_task_name(), ##__VA_ARGS__)
#define CORE_LOG_INFO(format, ...) core_log_write(CORE_LOG_INFO, CORE_LOG_FORMAT(I, format), core_log_get_timestamp(), core_log_get_task_name(), ##__VA_ARGS__)
#define CORE_LOG_DEBUG(format, ...) core_log_write(CORE_LOG_DEBUG, CORE_LOG_FORMAT(D, format), core_log_get_timestamp(), core_log_get_task_name(), ##__VA_ARGS__)
#define CORE_LOG_TRACE(format, ...) core_log_write(CORE_LOG_TRACE, CORE_LOG_FORMAT(T, format), core_log_get_timestamp(), core_log_get_task_name(), ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

typedef enum { CORE_LOG_ERROR, CORE_LOG_WARN, CORE_LOG_INFO, CORE_LOG_DEBUG, CORE_LOG_TRACE } core_log_level_t;

void core_log_write(core_log_level_t level, const char *format, ...) __attribute__((format(printf, 2, 3)));

uint32_t core_log_get_timestamp(void);

const char* core_log_get_task_name(void);

#endif// !_ESP32_LOGGER_
