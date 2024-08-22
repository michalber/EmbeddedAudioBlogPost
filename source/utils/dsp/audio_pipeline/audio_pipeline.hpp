#ifndef __UTILS_DSP_AUDIO_PIPELINE_HPP__
#define __UTILS_DSP_AUDIO_PIPELINE_HPP__

#include <array>
#include <tuple>
#include <vector>

#include "audio_processing_ctx.hpp"
#include "logger.hpp"

template<typename ProcessingCtx, typename... Processors>
class CAudioPipeline : public ProcessingCtx {
  public:
    CAudioPipeline() : mProcessors{} { ProcessingCtx::Prepare(mProcessors); }
    CAudioPipeline(const Processors &...t) : mProcessors(t...) { ProcessingCtx::Prepare(mProcessors); }

    ~CAudioPipeline() = default;

  private:
    std::tuple<Processors...>               mProcessors;
    std::array<bool, sizeof...(Processors)> mBypassCtl{ {} };
};

namespace std {
/** Adds support for C++17 structured bindings. */
template<typename ProcessingCtx, typename... Processors>
struct tuple_size<::CAudioPipeline<ProcessingCtx, Processors...>> : integral_constant<size_t, sizeof...(Processors)> {};

/** Adds support for C++17 structured bindings. */
template<size_t I, typename ProcessingCtx, typename... Processors>
struct tuple_element<I, ::CAudioPipeline<ProcessingCtx, Processors...>> : tuple_element<I, tuple<Processors...>> {};

}// namespace std

#endif// !__UTILS_DSP_AUDIO_PIPELINE_HPP__