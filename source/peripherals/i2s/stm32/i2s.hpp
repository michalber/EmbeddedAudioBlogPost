#ifndef __PERIPHS_STM32_I2S_HPP__
#define __PERIPHS_STM32_I2S_HPP__

#include <array>
#include <cstdint>
#include <optional>

#include "stm32f7xx_hal.h"

#include "helpers.hpp"
#include "types.h"

namespace driver::i2s {
enum class Dir { kOut, kIn };
enum class Mode { kMono, kStereo };
}// namespace driver::i2s

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE, std::size_t BLOCK_SIZE>
class CI2S {
  public:
    using kDirection = driver::i2s::Dir;
    using SampleType = uint8_t;

  public:
    CI2S(uint32_t rBclkGpio, uint32_t rWsGpio, uint32_t rDataGpio);

    ~CI2S() { HAL_I2S_DeInit(&mHandle); }

    void             Enable(bool rEnable);
    size_t           Write(const SampleType *rData, const std::size_t rDataCnt);
    size_t           Read(SampleType *rData, const std::size_t rDataCnt);
    driver::i2s::Dir GetDirection() const { return mDir; }


  private:
    std::size_t                          mSampleRate = FS;
    driver::i2s::Dir                     mDir        = DIR;
    driver::i2s::Mode                    mMode       = MODE;
    std::size_t                          mBlockSize  = BLOCK_SIZE;
    I2S_HandleTypeDef                    mHandle;
    uint32_t                             mBclkGpio;
    uint32_t                             mWsGpio;
    uint32_t                             mDataGpio;
    std::array<uint32_t, BLOCK_SIZE * 2> mDataBuf{};

    NO_COPY_OR_ASSIGN(CI2S);
};

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE, std::size_t BLOCK_SIZE>
CI2S<FS, DIR, MODE, BLOCK_SIZE>::CI2S(uint32_t rBclkGpio, uint32_t rWsGpio, uint32_t rDataGpio)
    : mBclkGpio{ rBclkGpio }, mWsGpio{ rWsGpio }, mDataGpio{ rDataGpio } {
    mHandle.Instance         = SPI2;
    mHandle.Init.Mode        = I2S_MODE_MASTER_TX;
    mHandle.Init.Standard    = I2S_STANDARD_PHILIPS;
    mHandle.Init.DataFormat  = I2S_DATAFORMAT_32B;
    mHandle.Init.MCLKOutput  = I2S_MCLKOUTPUT_ENABLE;
    mHandle.Init.AudioFreq   = mSampleRate;
    mHandle.Init.CPOL        = I2S_CPOL_LOW;
    mHandle.Init.ClockSource = I2S_CLOCK_PLL;
    HAL_I2S_Init(&mHandle);
}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE, std::size_t BLOCK_SIZE>
void CI2S<FS, DIR, MODE, BLOCK_SIZE>::Enable(bool rEnable) {}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE, std::size_t BLOCK_SIZE>
size_t CI2S<FS, DIR, MODE, BLOCK_SIZE>::Write(const CI2S<FS, DIR, MODE, BLOCK_SIZE>::SampleType *rData,
                                              const std::size_t                                  rDataCnt) {
    if (mDir == driver::i2s::Dir::kIn) { return 0; }

    CopyMonoBufferToInterleaved<uint32_t>(reinterpret_cast<const uint32_t *>(rData), mDataBuf.data(), rDataCnt);

    HAL_I2S_Transmit(&mHandle, (uint16_t *)mDataBuf.data(), rDataCnt * 2, HAL_MAX_DELAY);

    return rDataCnt;
}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE, std::size_t BLOCK_SIZE>
size_t CI2S<FS, DIR, MODE, BLOCK_SIZE>::Read(CI2S<FS, DIR, MODE, BLOCK_SIZE>::SampleType *rData,
                                             const std::size_t                            rDataCnt) {
    if (mDir == driver::i2s::Dir::kOut) { return 0; }

    HAL_I2S_Receive(&mHandle, (uint16_t *)mDataBuf.data(), rDataCnt * 2, HAL_MAX_DELAY);

    CopyInterleavedToMonoBuffer<uint32_t>(mDataBuf.data(), reinterpret_cast<uint32_t *>(rData), rDataCnt);

    return rDataCnt;
}

#endif// !__PERIPHS_STM32_I2S_HPP__
