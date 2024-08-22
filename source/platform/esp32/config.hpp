#ifndef __PLATFORM_ESP32_CONFIG_HPP__
#define __PLATFORM_ESP32_CONFIG_HPP__

#include "types.h"
#include <cstddef>
#include <cstdint>

constexpr std::size_t I2S_SAMPLING_FREQ = 48.0_kHz;
constexpr uint32_t    I2S_IN_BCLK_GPIO     = 33;
constexpr uint32_t    I2S_IN_WS_GPIO       = 25;
constexpr uint32_t    I2S_IN_DATA_GPIO     = 32;
constexpr uint32_t    I2S_OUT_BCLK_GPIO     = 17;
constexpr uint32_t    I2S_OUT_WS_GPIO       = 4;
constexpr uint32_t    I2S_OUT_DATA_GPIO     = 16;

#endif// !__PLATFORM_ESP32_CONFIG_HPP__