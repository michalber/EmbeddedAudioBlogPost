#ifndef __PERIPHS_ESP32_I2S_HPP__
#define __PERIPHS_ESP32_I2S_HPP__

#include <cstdint>
#include <optional>

#include "driver/i2s_std.h"
#include "esp_check.h"
#include "types.h"

namespace driver::i2s {
enum class Dir { kOut, kIn };
enum class Mode { kMono, kStereo };
}// namespace driver::i2s

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE>
class CI2S {
  public:
    using kDirection = driver::i2s::Dir;
    using SampleType = uint8_t;

  public:
    CI2S(uint32_t rBclkGpio, uint32_t rWsGpio, uint32_t rDataGpio);

    ~CI2S()
    {
        this->Enable(false);
        i2s_del_channel(mHandle);
    }

    void             Enable(bool rEnable);
    size_t           Write(const SampleType *rData, const std::size_t rDataCnt);
    size_t           Read(SampleType *rData, const std::size_t rDataCnt);
    driver::i2s::Dir GetDirection() const { return mDir; }


  private:
    std::size_t       mSampleRate = FS;
    driver::i2s::Dir  mDir        = DIR;
    driver::i2s::Mode mMode       = MODE;
    i2s_chan_handle_t mHandle;
    gpio_num_t        mBclkGpio;
    gpio_num_t        mWsGpio;
    gpio_num_t        mDataGpio;

    NO_COPY_OR_ASSIGN(CI2S);
};

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE>
CI2S<FS, DIR, MODE>::CI2S(uint32_t rBclkGpio, uint32_t rWsGpio, uint32_t rDataGpio)
    : mBclkGpio{ (gpio_num_t)rBclkGpio }, mWsGpio{ (gpio_num_t)rWsGpio }, mDataGpio{ (gpio_num_t)rDataGpio } {
    i2s_chan_config_t channelCfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);

    i2s_new_channel(&channelCfg,
                    mDir == driver::i2s::Dir::kOut ? &mHandle : nullptr,
                    mDir == driver::i2s::Dir::kIn ? &mHandle : nullptr);

    i2s_slot_mode_t slotMode;
    switch (mMode) {
    case driver::i2s::Mode::kMono:
        slotMode = I2S_SLOT_MODE_MONO;
        break;
    case driver::i2s::Mode::kStereo:
        slotMode = I2S_SLOT_MODE_STEREO;
        break;
    default:
        slotMode = I2S_SLOT_MODE_MONO;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(mSampleRate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, slotMode),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = mBclkGpio,
            .ws   = mWsGpio,
            .dout = mDir == driver::i2s::Dir::kOut ? mDataGpio : I2S_GPIO_UNUSED,
            .din  = mDir == driver::i2s::Dir::kIn ? mDataGpio : I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(mHandle, &std_cfg));
}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE>
void CI2S<FS, DIR, MODE>::Enable(bool rEnable) {
    if (rEnable) {
        ESP_ERROR_CHECK(i2s_channel_enable(mHandle));
    } else {
        ESP_ERROR_CHECK(i2s_channel_disable(mHandle));
    }
}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE>
size_t CI2S<FS, DIR, MODE>::Write(const CI2S<FS, DIR, MODE>::SampleType *rData, const std::size_t rDataCnt) {
    if (mDir == driver::i2s::Dir::kIn) { return 0; }

    size_t    bytesWritten = rDataCnt;
    esp_err_t err          = i2s_channel_write(mHandle, rData, rDataCnt, &bytesWritten, 10000);
    if (err != ESP_OK) { return 0; }

    return bytesWritten;
}

template<std::size_t FS, driver::i2s::Dir DIR, driver::i2s::Mode MODE>
size_t CI2S<FS, DIR, MODE>::Read(CI2S<FS, DIR, MODE>::SampleType *rData, const std::size_t rDataCnt) {
    if (mDir == driver::i2s::Dir::kOut) { return 0; }

    size_t    bytesRead = rDataCnt;
    esp_err_t err       = i2s_channel_read(mHandle, rData, rDataCnt, &bytesRead, 10000);
    if (err != ESP_OK) { return 0; }

    return bytesRead;
}

#endif// !__PERIPHS_ESP32_I2S_HPP__
