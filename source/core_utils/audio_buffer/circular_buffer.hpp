#ifndef __CORE_UTILS_CIRC_BUF_HPP__
#define __CORE_UTILS_CIRC_BUF_HPP__

#include "types.h"

#include <array>
#include <atomic>
#include <cstdlib>

template<typename T, std::size_t N>
class CCircularBuffer {
    static_assert(std::is_trivial<T>::value, "The type T must be trivial");
    static_assert(N > 2, "Buffer size must be bigger than 2");
    static_assert((N & (N - 1)), "Buffer size must be power of 2");

  public:
    CCircularBuffer();
    ~CCircularBuffer() = default;

    uint32_t GetLevel();
    uint32_t GetFreeSize();
    uint32_t GetSize();
    uint32_t Push(T &rVal);
    uint32_t Write(const T *rData, const std::size_t rCnt);
    uint32_t Pop(T &rVal);
    uint32_t Read(T *rData, const std::size_t rCnt);
    uint32_t Flush();

  private:
    uint32_t SetHead(uint32_t rHead) { return rHead & (mSize - 1); }

    uint32_t SetTail(uint32_t rTail) { return mTail & (mSize - 1); }

    uint32_t AdvIdx(uint32_t rIdx, uint32_t rVal) { return ((rIdx + rVal) & (mSize - 1)); }

    std::size_t CalcFree(const std::size_t w, const std::size_t r) {
        if (r > w) {
            return (r - w) - 1U;
        } else {
            return (mSize - (w - r)) - 1U;
        }
    }

    std::size_t CalcAvailable(const std::size_t w, const std::size_t r) {
        if (w >= r) {
            return w - r;
        } else {
            return mSize - (r - w);
        }
    }

  private:
    std::atomic_size_t mHead{0};
    std::atomic_size_t mTail{0};
    const uint32_t     mSize{N};
    std::array<T, N>   mData{};

    NO_COPY_OR_ASSIGN(CCircularBuffer);
    DEFAULT_MOVE(CCircularBuffer)
};

template<typename T, std::size_t N>
CCircularBuffer<T, N>::CCircularBuffer() : mHead{0}, mTail{0}, mSize{N}, mData{} {}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::GetLevel() {
    return ((mTail - mHead) & (mSize - 1));
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::GetFreeSize() {
    return ((mSize - mTail + mHead - 1) & (mSize - 1));
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::GetSize() {
    return (mSize - 1);
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::Push(T &rVal) {
    /* Preload variables with adequate memory ordering */
    auto       dTail = mTail.load(std::memory_order_relaxed);
    const auto dHead = mHead.load(std::memory_order_acquire);

    if (CalcFree(dTail, dHead) < 1) { return 1; }

    auto dTemp = AdvIdx(dTail, 1);

    if (dTemp != dHead) {
        mData[dTail] = rVal;
        dTail        = dTemp;
    } else {
        return 1;
    }

    mTail.store(SetTail(dTail), std::memory_order_release);

    return 0;
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::Write(const T *rData, const std::size_t rCnt) {
    /* Preload variables with adequate memory ordering */
    auto       dTail = mTail.load(std::memory_order_relaxed);
    const auto dHead = mHead.load(std::memory_order_acquire);

    if (CalcFree(dTail, dHead) < rCnt) { return 1; }

    /* Check if the write wraps */
    if (dTail + rCnt <= mSize) {
        /* Copy in the linear region */
        memcpy(&mData[dTail], &rData[0], rCnt * sizeof(T));
        /* Correct the write index */
        dTail = AdvIdx(dTail, rCnt);
    } else {
        /* Copy in the linear region */
        const std::size_t linear_free = mSize - dTail;
        memcpy(&mData[dTail], &rData[0], linear_free * sizeof(T));
        /* Copy remaining to the beginning of the buffer */
        const std::size_t remaining = rCnt - linear_free;
        memcpy(&mData[0], &rData[linear_free], remaining * sizeof(T));
        /* Correct the write index */
        dTail = SetTail(remaining);
    }

    /* Store the write index with adequate ordering */
    mTail.store(dTail, std::memory_order_release);

    return 0;
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::Pop(T &rVal) {
    /* Preload variables with adequate memory ordering */
    auto       dHead = mHead.load(std::memory_order_relaxed);
    const auto dTail = dTail.load(std::memory_order_acquire);

    if (CalcAvailable(dTail, dHead) < 1) { return 1; }

    auto dTemp = AdvIdx(dHead, 1);

    if (dTail != dHead) {
        rVal = mData[dHead];
        mHead.store(SetHead(dTemp), std::memory_order_release);
        return 0;
    }

    return 1;
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::Read(T *rData, const std::size_t rCnt) {
    /* Preload variables with adequate memory ordering */
    auto       dHead = mHead.load(std::memory_order_relaxed);
    const auto dTail = mTail.load(std::memory_order_acquire);

    if (CalcAvailable(dTail, dHead) < rCnt) { return 1; }

    /* Check if the read wraps */
    if (dHead + rCnt <= mSize) {
        /* Copy in the linear region */
        memcpy(&rData[0], &mData[dHead], rCnt * sizeof(T));
        /* Correct the read index */
        dHead = AdvIdx(dHead, rCnt);
    } else {
        /* Copy in the linear region */
        const std::size_t linear_available = mSize - dHead;
        memcpy(&rData[0], &mData[dHead], linear_available * sizeof(T));
        /* Copy remaining from the beginning of the buffer */
        const std::size_t remaining = rCnt - linear_available;
        memcpy(&rData[linear_available], &mData[0], remaining * sizeof(T));
        /* Correct the read index */
        dHead = SetHead(remaining);
    }

    /* Store the write index with adequate ordering */
    mHead.store(dHead, std::memory_order_release);

    return 0;
}

template<typename T, std::size_t N>
uint32_t CCircularBuffer<T, N>::Flush() {
    auto dLevel = GetLevel();
    mHead       = 0;
    mTail       = 0;
    return dLevel;
}

#endif// !__CORE_UTILS_CIRC_BUF_HPP__