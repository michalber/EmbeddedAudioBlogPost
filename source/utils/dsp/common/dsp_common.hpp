#ifndef __UTILS_DSP_COMMON_HPP__
#define __UTILS_DSP_COMMON_HPP__

#include "dsp_types.hpp"

#include <array>

namespace dsp {

struct HammingWindow {
    template<std::size_t N> static constexpr std::array<float, N> Get() {
        std::array<float, N> tHammingWindow{};
        constexpr auto       alpha = 0.54f;
        constexpr auto       beta  = 0.46f;

        uint16_t i = 0;
        for (auto &el : tHammingWindow) { el = alpha - beta * std::cos(kTwoPi * i++ / (N - 1)); }

        return tHammingWindow;
    }
};

struct HanningWindow {
    template<std::size_t N> static constexpr std::array<float, N> Get() {
        std::array<float, N> tHanningWindow{};
        uint16_t             i = 0;
        for (auto &el : tHanningWindow) {
            el = std::sin((kPi * i) / (N - 1)) * std::sin((kPi * i) / (N - 1));
            ++i;
        }
        return tHanningWindow;
    }
};

struct BlackmanWindow {
    template<std::size_t N> static constexpr std::array<float, N> Get() {
        std::array<float, N> tBlackmanWindow{};
        constexpr auto       alpha0 = 0.42;
        constexpr auto       alpha1 = 0.5;
        constexpr auto       alpha2 = 0.08;
        uint16_t             i      = 0;
        for (auto &el : tBlackmanWindow) {
            el = alpha0 - alpha1 * std::cos(2.0 * kPi * i / (N - 1)) - alpha2 * std::cos(4.0 * kPi * i / (N - 1));
            ++i;
        }
        return tBlackmanWindow;
    }
};

namespace math {
    template<typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    constexpr inline T sinc(const T x) {
        if (x == 0) return 1;

        return std::sin(kPi * x) / (kPi * x);
    }
}

}// namespace dsp

#endif// !__UTILS_DSP_COMMON_HPP__