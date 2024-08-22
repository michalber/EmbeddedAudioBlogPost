#ifndef __ESP32_PLATFORM__
#define __ESP32_PLATFORM__

#include <algorithm>
#include <functional>
#include <numeric>

#include "audio_pipeline.hpp"
#include "config.hpp"
#include "i2s.hpp"
#include "i2s_processor.hpp"
#include "lifetime_obj.hpp"
#include "logger.hpp"
#include "platform.hpp"

using SampleType = int32_t;

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

class CPlatform_ESP32 : public CPlatform {
  public:
    using CI2S_Input  = CI2S<I2S_SAMPLING_FREQ, driver::i2s::Dir::kIn, driver::i2s::Mode::kMono>;
    using CI2S_Output = CI2S<I2S_SAMPLING_FREQ, driver::i2s::Dir::kOut, driver::i2s::Mode::kMono>;

  public:
    CPlatform_ESP32();
    ~CPlatform_ESP32();

    void InitDrivers() override;

    void InitPlatform() override;

  private:
    static inline constexpr auto kBlockSize = 8;

    using ProcessingCtx = SingleThreadedProcessingCtx<SampleType, 1, kBlockSize>;
    // using ProcessingCtx = MultiThreadedProcessingCtx<SampleType, 1, kBlockSize>;

    using InputReader =
        CI2sProcessor<CI2S_Input, I2sReadPolicy<SampleType, 14>, SampleType, kBlockSize, ProcessingType::kReadOnly>;
    using OutputWriter =
        CI2sProcessor<CI2S_Output, I2sWritePolicy<SampleType>, SampleType, kBlockSize, ProcessingType::kWriteOnly>;

    CAudioPipeline<ProcessingCtx,
                   InputReader,
                   Passthrough<SampleType, kBlockSize>,
                   Gain<SampleType, kBlockSize>,
                   OutputWriter>
        mAudioPipeline;
};

#endif// !__ESP32_PLATFORM__