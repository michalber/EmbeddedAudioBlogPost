#include "assert.hpp"

#include "platform.hpp"

#include <cassert>
#include <cstdio>

void coreAssertFail(uint32_t rdLine, const char *rtFileName, void *rpCaller, int rdOpt)
{
    char str[36];

    snprintf(str, sizeof(str), "CORE c:%08X o:%d", (unsigned int)(uintptr_t)rpCaller, rdOpt);

    CPlatform::Assert(str, rtFileName, rdLine);
}
