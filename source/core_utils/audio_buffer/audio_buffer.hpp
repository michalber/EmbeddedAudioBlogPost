#ifndef __CORE_UTILS_AUDIO_BUFFER_HPP__
#define __CORE_UTILS_AUDIO_BUFFER_HPP__

#include "types.h"

#include <cstdlib>
#include <array>

template<typename T, std::size_t CH_NUM, std::size_t BUF_N>
class CAudioBuffer {
  public:
    using CChannel = std::array<T, BUF_N>;

  public:
    CAudioBuffer() : mData{} {
        static_assert((BUF_N % 2) == 0, "The CAudioBuffer BUF_N must be even");
        Clear();
    }

    ~CAudioBuffer() = default;

    // Returns the number of audio channels.
    inline std::size_t NumChannels() const { return mData.size(); }

    // Returns the number of frames per buffer.
    inline std::size_t NumFrames() const { return mData[0].size(); }

    // Returns a reference to the selected ChannelView.
    inline CChannel &operator[](std::size_t channel) {
        //	DCHECK_LT(channel, channel_views_.size());
        return mData[channel];
    }

    // Returns a const reference to the selected ChannelView.
    inline const CChannel &operator[](std::size_t channel) const {
        //	DCHECK_LT(channel, channel_views_.size());
        return mData[channel];
    }

    inline const T *ReadPointer(const unsigned int channelNumber) const { return mData[channelNumber].data(); };

    inline T *WritePointer(const unsigned int channelNumber) { return mData[channelNumber].data(); };

    inline std::array<CChannel, CH_NUM> &Data() const { return mData; };

    inline std::array<CChannel, CH_NUM> &Data() { return mData; };

    // += operator
    CAudioBuffer &operator+=(const CAudioBuffer &other) {
        //	DCHECK_EQ(other.num_channels(), num_channels());
        //	DCHECK_EQ(other.num_frames(), num_frames());
        for (std::size_t ch = 0; ch < mData.size(); ++ch) {
            for (std::size_t i = 0; i < mData[ch].size(); ++i) { mData[ch][i] += other[ch][i]; }
        }
        return *this;
    }

    // -= operator
    CAudioBuffer &operator-=(const CAudioBuffer &other) {
        //	DCHECK_EQ(other.num_channels(), num_channels());
        //	DCHECK_EQ(other.num_frames(), num_frames());
        for (std::size_t ch = 0; ch < mData.size(); ++ch) {
            for (std::size_t i = 0; i < mData[ch].size(); ++i) { mData[ch][i] -= other[ch][i]; }
        }
        return *this;
    }

    // *= operator
    CAudioBuffer &operator*=(const CAudioBuffer &other) {
        //	DCHECK_EQ(other.num_channels(), num_channels());
        //	DCHECK_EQ(other.num_frames(), num_frames());
        for (std::size_t ch = 0; ch < mData.size(); ++ch) {
            for (std::size_t i = 0; i < mData[ch].size(); ++i) { mData[ch][i] *= other[ch][i]; }
        }
        return *this;
    }

    // /= operator
    CAudioBuffer &operator/=(const CAudioBuffer &other) {
        //	DCHECK_EQ(other.num_channels(), num_channels());
        //	DCHECK_EQ(other.num_frames(), num_frames());
        for (std::size_t ch = 0; ch < mData.size(); ++ch) {
            for (std::size_t i = 0; i < mData[ch].size(); ++i) { mData[ch][i] /= other[ch][i]; }
        }
        return *this;
    }

    // Returns an iterator to the ChannelView of the first channel.
    auto begin() { return mData.begin(); }

    // Returns an iterator to the end of the ChannelView vector.
    auto end() { return mData.end(); }

    // Returns a const_iterator to the ChannelView of the first channel.
    auto begin() const { return mData.cbegin(); }

    // Returns an const_iterator to the end of the ChannelView vector.
    auto end() const { return mData.cend(); }

    // Fills all channels with zeros and reenables |Channel|s.
    void Clear() {
        for (auto &channel : mData) { channel.fill(0); }
    }

  private:
    std::array<CChannel, CH_NUM> mData;

    NO_COPY_OR_ASSIGN(CAudioBuffer);
};

#endif// !__CORE_UTILS_AUDIO_BUFFER_HPP__