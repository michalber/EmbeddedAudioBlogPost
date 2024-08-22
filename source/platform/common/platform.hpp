#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

// #include "osal.h"
#include "singleton.hpp"
#include "types.h"

#define CPlatformI (CPlatform::GetInstance())

class CPlatform : public core::Singleton<CPlatform>
{
public:
    CPlatform() : core::Singleton<CPlatform>{this}
    {}
    
    virtual ~CPlatform() = default;
    
    static void Assert(const char *rpMsg, const char *rpFileName, int rdLine);

    static void LaunchPlatform();

    virtual void InitDrivers() = 0;

    virtual void InitPlatform() = 0;

    NO_COPY_OR_ASSIGN(CPlatform);
};

#endif // __PLATFORM_HPP__
