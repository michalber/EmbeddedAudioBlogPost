#ifndef __UTILS_DPS_AUDIO_PROC_I2S_HPP__
#define __UTILS_DPS_AUDIO_PROC_I2S_HPP__

#include "audio_processor.hpp"
#include "lifetime_obj.hpp"
#include <limits>

template<typename SampleType, std::size_t BitShift>
struct I2sReadPolicy {
    inline SampleType operator()(SampleType sample) { return sample >> BitShift; }
};

template<typename SampleType>
struct I2sWritePolicy {
    inline SampleType operator()(SampleType sample) { return sample; }
};


template<class I2sInstance, class DataPolicy, typename T, std::size_t N, ProcessingType Dir>
class CI2sProcessor : public CAudioProcessor<T, N> {
  public:
    CI2sProcessor(CLifetimeObj<I2sInstance> &rI2s) : mI2sInstance{ rI2s } {}
    ~CI2sProcessor() {}

    void ProcessBlock(std::array<T, N> &input, std::array<T, N> &output, uint32_t samples) override {
        auto &i2s = LifetimeObjGet(mI2sInstance);

        if constexpr(Dir == ProcessingType::kReadOnly || Dir == ProcessingType::kReadWrite)
        {
            i2s.Read(reinterpret_cast<typename I2sInstance::SampleType *>(output.data()), samples);
            // for(auto &el : output)
            // {
            //     // el = mDataPolicy(el);
            // }
        }
        else if constexpr(Dir == ProcessingType::kWriteOnly || Dir == ProcessingType::kReadWrite)
        {
            i2s.Write(reinterpret_cast<const typename I2sInstance::SampleType *>(input.data()), samples);
            // for(auto &el : input)
            // {
                // el = mDataPolicy(el);
            // }
        }
    }

  private:
    CLifetimeObj<I2sInstance> &mI2sInstance;
    DataPolicy                 mDataPolicy;
};

#endif// !__UTILS_DPS_AUDIO_PROC_I2S_HPP__