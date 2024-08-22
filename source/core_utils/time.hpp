#ifndef __UTILS_TIME_HPP__
#define __UTILS_TIME_HPP__

#define HOURS_IN_DAY 24
#define SEC_IN_MIN 60
#define MIN_IN_HOUR 60
#define SEC_IN_HOUR (SEC_IN_MIN * MIN_IN_HOUR)
#define SEC_IN_DAY (SEC_IN_HOUR * HOURS_IN_DAY)

#define MILLIS_PER_SEC 1000
#define USEC_PER_MSEC 1000

#define MSEC_TO_USEC(_msec) (_msec * USEC_PER_MSEC)

#endif // !__UTILS_TIME_HPP__