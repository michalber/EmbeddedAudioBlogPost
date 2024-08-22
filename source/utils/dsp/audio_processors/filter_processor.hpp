#ifndef __UTILS_DPS_AUDIO_PROC_FILTER_HPP__
#define __UTILS_DPS_AUDIO_PROC_FILTER_HPP__

#include "audio_processor.hpp"

template<class FilterType, typename T, std::size_t N>
class CFilterProcessor : public CAudioProcessor<T, N> {
  public:
    CFilterProcessor(/* args */)  = default;
    ~CFilterProcessor() = default;

    void ProcessBlock(std::array<T, N> &input, std::array<T, N> &output, uint32_t samples) override {
        mFilter.ProcessBlock(input.data(), output.data(), samples);
    }

  private:
    FilterType mFilter;
};

#endif// !__UTILS_DPS_AUDIO_PROC_FILTER_HPP__