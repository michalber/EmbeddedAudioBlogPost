#ifndef __STM32_PLATFORM__
#define __STM32_PLATFORM__

#include <algorithm>
#include <functional>
#include <numeric>

#include "audio_pipeline.hpp"
#include "config.hpp"
#include "i2s.hpp"
#include "i2s_processor.hpp"
#include "lifetime_obj.hpp"
// #include "logger.hpp"
#include "platform.hpp"
// #include "timer.hpp"

using SampleType = int32_t;
static constexpr auto kBlockSize = 8;

template<typename T, std::size_t N>
class Passthrough : public CAudioProcessor<T, N> {
    void ProcessBlock(std::array<T, N> &input, std::array<T, N> &output, uint32_t samples) override {
        std::copy(input.begin(), input.end(), output.begin());
    }
};

template<typename T, std::size_t N>
class Gain : public CAudioProcessor<T, N> {
    void ProcessBlock(std::array<T, N> &input, std::array<T, N> &output, uint32_t samples) override {
        std::transform(
            input.begin(), input.end(), output.begin(), std::bind(std::multiplies(), std::placeholders::_1, 2.0f));
    }
};

class CPlatform_STM32 : public CPlatform {
  public:
    CPlatform_STM32();
    ~CPlatform_STM32();

    void InitDrivers() override;

    void InitPlatform() override;

  private:

};

#endif// !__STM32_PLATFORM__