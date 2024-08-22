#ifndef __UTILS_DSP_AUDIO_PROC_HPP__
#define __UTILS_DSP_AUDIO_PROC_HPP__

#include <array>
#include <cstdlib>

enum class ProcessingType
{
  kReadOnly,
  kReadWrite,
  kWriteOnly,
};

template<typename T, std::size_t N, ProcessingType Type = ProcessingType::kReadWrite>
class CAudioProcessor {
  public:
    static constexpr inline ProcessingType kProcessingType = Type;

    virtual ~CAudioProcessor()                                                                      = default;
    virtual void ProcessBlock(std::array<T, N> &rInput, std::array<T, N> &rOutput, uint32_t rSamples) = 0;
};

#endif// !__UTILS_DSP_AUDIO_PROC_HPP__