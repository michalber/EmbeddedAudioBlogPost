#ifndef __UTILS_DSP_AUDIO_PROC_CTX_HPP__
#define __UTILS_DSP_AUDIO_PROC_CTX_HPP__

#include "audio_buffer.hpp"
#include "circular_buffer.hpp"
#include "thread.hpp"
#include "audio_processor.hpp"

namespace helper {
template<typename Fn, typename Tuple, size_t... Ix>
constexpr void ForEachInTuple(Fn &&fn, Tuple &&tuple, std::index_sequence<Ix...>) {
    (fn(std::get<Ix>(tuple), std::integral_constant<size_t, Ix>()), ...);
}

template<typename T>
using TupleIndexSequence = std::make_index_sequence<std::tuple_size_v<std::remove_cv_t<std::remove_reference_t<T>>>>;

template<typename Fn, typename Tuple>
constexpr void ForEachInTuple(Fn &&fn, Tuple &&tuple) {
    ForEachInTuple(std::forward<Fn>(fn), std::forward<Tuple>(tuple), TupleIndexSequence<Tuple>{});
}

template<typename Context, size_t Ix>
inline constexpr auto useContextDirectly = !Context::usesSeparateInputAndOutputBlocks() || Ix == 0;
}// namespace helper


template<typename SampleType, std::size_t ChannelNum, std::size_t BufSize>
class SingleThreadedProcessingCtx {
  public:
    using AudioBufferType    = CAudioBuffer<SampleType, ChannelNum, BufSize>;
    using AudioProcessorType = CAudioProcessor<SampleType, BufSize>;

    static inline constexpr auto kChannelNum = ChannelNum;
    static inline constexpr auto kBufSize    = BufSize;

  public:
    SingleThreadedProcessingCtx()
        : mInputBuffer{ &mBuffer1 }, mOutputBuffer{ &mBuffer2 },
          mProcessingThread{ SingleThreadedProcessingCtx::ProcessingTask,
                             this,
                             "STAudioThread",
                             OSAL_PRIO_HIGH,
                             0x1000 } {}

    ~SingleThreadedProcessingCtx() {
        if (mProcessingThread.IsRunning()) { mProcessingThread.Stop(); }
    }

    template<typename ProcessorsTuple>
    void Prepare(const ProcessorsTuple &rProcessors) {
        mProcessors.reserve(std::tuple_size_v<ProcessorsTuple>);
        helper::ForEachInTuple([this](auto &proc, auto) { this->mProcessors.push_back((AudioProcessorType *)(&proc)); },
                               rProcessors);
    }

    void Run() { mProcessingThread.Start(); }

    void Pause() { mProcessingThread.Suspend(); }


  private:
    static void ProcessingTask(void *rCtx) {
        SingleThreadedProcessingCtx *thisPtr = static_cast<SingleThreadedProcessingCtx *>(rCtx);
        while (thisPtr->mProcessingThread.IsRunning()) { thisPtr->Process(); }
    }

    void Process() {
        for (auto *processor : mProcessors) {
            for (std::size_t i = 0; i < kChannelNum; ++i) {
                processor->ProcessBlock((*mInputBuffer)[i], (*mOutputBuffer)[i], kBufSize);
            }
            std::swap(mInputBuffer, mOutputBuffer);
        }
    }

  private:
    AudioBufferType                   mBuffer1, mBuffer2;
    AudioBufferType                  *mInputBuffer, *mOutputBuffer;
    CThread                           mProcessingThread;
    std::vector<AudioProcessorType *> mProcessors;
};


template<typename SampleType, std::size_t ChannelNum, std::size_t BufSize>
struct MultiThreadedProcessingCtx {
    static inline constexpr auto kChannelNum = ChannelNum;
    static inline constexpr auto kBufSize    = BufSize;

    using AudioProcessorType = CAudioProcessor<SampleType, BufSize>;
    using AudioBufferType    = CAudioBuffer<SampleType, ChannelNum, BufSize>;

  public:
    MultiThreadedProcessingCtx() {}
    ~MultiThreadedProcessingCtx() {}

    template<typename ProcessorsTuple>
    void Prepare(const ProcessorsTuple &rProcessors) {
        mBuffers.reserve(std::tuple_size_v<ProcessorsTuple>);
        // mThreadCtxs.reserve(std::tuple_size_v<ProcessorsTuple>);
        // helper::ForEachInTuple(
        //     [this](auto &proc, auto i) { this->mThreadCtxs.push_back(ProcessingThreadCtx{ proc, mBuffers[i / 2], "MPTask" + i }); },
        //     rProcessors);
    }

    void Run() {
        // for (auto &ctx : mThreadCtxs) { ctx.mThread.Start(); }
    }

    void Pause() {
        // for (auto &ctx : mThreadCtxs) { ctx.mThread.Suspend(); }
    }


  private:
    static inline auto constexpr kBufferMultiplier = 3;
    using TaskCircBuffer = CCircularBuffer<SampleType, ChannelNum * BufSize * kBufferMultiplier>;

    // class ProcessingThreadCtx {
    //   public:
    //     ProcessingThreadCtx(AudioProcessorType *rProcessor, const TaskCircBuffer &rCircBuffer, const char *rName)
    //         : mThread{ ProcessingThreadCtx::ProcessingTask, this, rName, OSAL_PRIO_HIGH, 0x1000 },
    //           mProcessor{ rProcessor }, mCircBuffer{ rCircBuffer } {}

    //   private:
    //     static void ProcessingTask(void *rCtx) {
    //         ProcessingThreadCtx *thisPtr = static_cast<ProcessingThreadCtx *>(rCtx);
    //         while (thisPtr->mThread.IsRunning()) { thisPtr->Process(); }
    //     }

    //     void Process() {
    //         for (int i = 0; i < kChannelNum; ++i) {
    //             // If AudioProcessor is made for read/rw, read data from buffer
    //             if constexpr (AudioProcessorType::kProcessingType == ProcessingType::kReadOnly
    //                           || AudioProcessorType::kProcessingType == ProcessingType::kReadWrite) {
    //                 mCircBuffer.Read(mInputBuffer[i].data(), kBufSize);
    //             }

    //             // Process data with processor
    //             mProcessor->ProcessBlock(mInputBuffer[i], mOutputBuffer[i], kBufSize);

    //             // If AudioProcessor is made for write/rw, write data to buffer
    //             if constexpr (AudioProcessorType::kProcessingType == ProcessingType::kWriteOnly
    //                           || AudioProcessorType::kProcessingType == ProcessingType::kReadWrite) {
    //                 mCircBuffer.Write(mOutputBuffer[i].data(), kBufSize);
    //             }
    //         }
    //     }

    //   public:
    //     CThread mThread;

    //   private:
    //     TaskCircBuffer     &mCircBuffer;
    //     AudioBufferType     mInputBuffer{}, mOutputBuffer{};
    //     AudioProcessorType *mProcessor{nullptr};
    // };

    // std::vector<ProcessingThreadCtx>  mThreadCtxs{};
    std::vector<TaskCircBuffer> mBuffers{};
    std::vector<AudioProcessorType *> mProcessors{};
};

#endif// !__UTILS_DSP_AUDIO_PROC_CTX_HPP__