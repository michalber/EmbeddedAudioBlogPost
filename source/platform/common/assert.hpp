#ifndef __CORE_ASSERT__
#define __CORE_ASSERT__

#include <cstdint>

#ifdef __GNU_C__
#define CORE_ASSERT_GET_CALLER __builtin_return_address(0)
#else
#define CORE_ASSERT_GET_CALLER (NULL)
#endif

#define CORE_ASSERT_OPT(_cond, _opt) \
if(!(_cond)) \
{ \
    coreAssertFail(__LINE__, __FILENAME__, CORE_ASSERT_GET_CALLER, _opt); \
}

#define CORE_ASSERT(_condition) CORE_ASSERT_OPT(_condition , 0)

void coreAssertFail(uint32_t rdLine, const char *rtFileName, void *rpCaller, int rdOpt);

#endif // !__CORE_ASSERT__