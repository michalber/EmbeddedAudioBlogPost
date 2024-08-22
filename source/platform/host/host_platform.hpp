#ifndef __HOST_PLATFORM__
#define __HOST_PLATFORM__

#include <algorithm>
#include <functional>
#include <numeric>

#include "platform.hpp"

using SampleType = int32_t;
static constexpr auto kBlockSize = 8;

class CPlatform_Host : public CPlatform {
  public:
    CPlatform_Host();
    ~CPlatform_Host();

    void InitDrivers() override;

    void InitPlatform() override;

  private:

};

#endif //!__HOST_PLATFORM__