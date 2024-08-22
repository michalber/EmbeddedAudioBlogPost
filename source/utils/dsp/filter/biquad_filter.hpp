#ifndef __DSP_BIQUAD_FILT_HPP__
#define __DSP_BIQUAD_FILT_HPP__

#include <array>

#include "dsp_types.hpp"


namespace dsp::biquad {
static inline constexpr auto kBiquadArraySize = 5;

typedef struct {
    float f0;
    float fs;
    float gain_db;
    float q;
} kEqParams;

struct LPF {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2.0f * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{}, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = 0.5 * (1.0 - cos_w);
        b1 = 1.0 - cos_w;
        b2 = b0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cos_w;
        a2 = 1.0 - alpha;

        tBiquadCoeffs = { { static_cast<T>(b0 / a0),
                            static_cast<T>(b1 / a0),
                            static_cast<T>(b2 / a0),
                            static_cast<T>(a1 / a0),
                            static_cast<T>(a2 / a0) } };

        return tBiquadCoeffs;
    }
};


struct HPF {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = 0.5 * (1.0 - cos_w);
        b1 = -(1.0 - cos_w);
        b2 = b0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cos_w;
        a2 = 1.0 - alpha;

        tBiquadCoeffs = { T{ b0 / a0 }, T{ b1 / a0 }, T{ b2 / a0 }, T{ a1 / a0 }, T{ a2 / a0 } };

        return tBiquadCoeffs;
    }
};


struct BPF {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = alpha;
        b1 = 0.0;
        b2 = -b0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cos_w;
        a2 = 1.0 - alpha;

        tBiquadCoeffs = { T{ b0 / a0 }, T{ b1 / a0 }, T{ b2 / a0 }, T{ a1 / a0 }, T{ a2 / a0 } };

        return tBiquadCoeffs;
    }
};

struct Notch {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = 1.0;
        b1 = -2.0 * cos_w;
        b2 = b0;
        a0 = 1.0 + alpha;
        a1 = -2.0 * cos_w;
        a2 = 1.0 - alpha;

        tBiquadCoeffs = { T{ b0 / a0 }, T{ b1 / a0 }, T{ b2 / a0 }, T{ a1 / a0 }, T{ a2 / a0 } };

        return tBiquadCoeffs;
    }
};

struct Peak {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = 1.0 + alpha * A;
        b1 = -2.0 * cos_w;
        b2 = 1.0 - alpha * A;
        a0 = 1.0 + alpha / A;
        a1 = -2.0 * cos_w;
        a2 = 1.0 - alpha / A;

        tBiquadCoeffs = { static_cast<float>(b0 / a0),
                          static_cast<float>(b1 / a0),
                          static_cast<float>(b2 / a0),
                          static_cast<float>(a1 / a0),
                          static_cast<float>(a2 / a0) };

        return tBiquadCoeffs;
    }
};


struct LowShelf {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = A * ((A + 1.0) - (A - 1.0) * cos_w + beta);
        b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cos_w);
        b2 = A * ((A + 1.0) - (A - 1.0) * cos_w - beta);
        a0 = (A + 1.0) + (A - 1.0) * cos_w + beta;
        a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cos_w);
        a2 = (A + 1.0) + (A - 1.0) * cos_w - beta;

        tBiquadCoeffs = { T{ b0 / a0 }, T{ b1 / a0 }, T{ b2 / a0 }, T{ a1 / a0 }, T{ a2 / a0 } };

        return tBiquadCoeffs;
    }
};


struct HighShelf {
    template<typename T = float>
    constexpr std::array<T, kBiquadArraySize> GetCoeffs(const kEqParams &params) {
        std::array<T, kBiquadArraySize> tBiquadCoeffs{};

        const float A     = std::pow(10.0, params.gain_db / 40.0);
        const float w     = 2 * M_PI * params.f0 / params.fs;
        const float sin_w = std::sin(w);
        const float cos_w = std::cos(w);
        const float alpha = 0.5 * sin_w / params.q;
        const float beta  = 2.0 * alpha / std::sqrt(A);

        float a0{ 1.0 }, a1{}, a2{}, b0{}, b1{}, b2{};

        b0 = A * ((A + 1.0) + (A - 1.0) * cos_w + beta);
        b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cos_w);
        b2 = A * ((A + 1.0) + (A - 1.0) * cos_w - beta);
        a0 = (A + 1.0) - (A - 1.0) * cos_w + beta;
        a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cos_w);
        a2 = (A + 1.0) - (A - 1.0) * cos_w - beta;

        tBiquadCoeffs = { T{ b0 / a0 }, T{ b1 / a0 }, T{ b2 / a0 }, T{ a1 / a0 }, T{ a2 / a0 } };

        return tBiquadCoeffs;
    }
};

template<typename T>
class BiquadProcessingBasic {
  public:
    template<std::size_t N>
    BiquadProcessingBasic(const std::array<T, N> &&coeffs) : mCoeffs{ coeffs } {}

  public:
    inline void ProcessBlock(const T *input, T *output, int32_t samples) {
        for (int i = 0; i < samples; ++i) { output[i] = ProcessSample(input[i]); }
    }

    inline T ProcessSample(T sample) {
        constexpr uint8_t b0 = 0;
        constexpr uint8_t b1 = 1;
        constexpr uint8_t b2 = 2;
        constexpr uint8_t a1 = 3;
        constexpr uint8_t a2 = 4;

        const T output = mCoeffs[b0] * sample + mWz0;
        mWz0           = mCoeffs[b1] * sample - mCoeffs[a1] * output + mWz1;
        mWz1           = mCoeffs[b2] * sample - mCoeffs[a2] * output;

        return output;
    }

  public:
    std::array<T, dsp::biquad::kBiquadArraySize> mCoeffs;
    T                                            mWz0;
    T                                            mWz1;
};

namespace simd {
    template<typename T>
    class BiquadProcessingSimd {
      public:
        template<std::size_t N>
        BiquadProcessingSimd(const std::array<T, N> &&coeffs) : mCoeffs{ coeffs } {}

      public:
        inline void ProcessBlock(const T *input, T *output, int32_t samples) {
            for (int i = 0; i < samples; ++i) { output[i] = ProcessSample(input[i]); }
        }

        inline T ProcessSample(T sample) {
            constexpr uint8_t b0 = 0;
            constexpr uint8_t b1 = 1;
            constexpr uint8_t b2 = 2;
            constexpr uint8_t a1 = 3;
            constexpr uint8_t a2 = 4;

            const T output = sample * mCoeffs[b0] + mWz0;
            mWz0           = sample * mCoeffs[b1] - mCoeffs[a1] * output + mWz1;
            mWz1           = sample * mCoeffs[b2] - mCoeffs[a2] * output;

            return output;
        }

      public:
        std::array<T, dsp::biquad::kBiquadArraySize> mCoeffs;
        T                                            mWz0;
        T                                            mWz1;
    };
}// namespace simd
}// namespace dsp::biquad


template<typename T, class FilterType, class ProcessingPolicy = dsp::biquad::BiquadProcessingBasic<T>>
class CBiquadFilter
    : public FilterType
    , public ProcessingPolicy {
  public:
    // explicit constexpr CBiquadFilter(const dsp::biquad::kEqParams &params)
    //     : ProcessingPolicy{ std::move(FilterType::template GetCoeffs<N, T>(params)) } {}

    template<std::size_t N>
    explicit constexpr CBiquadFilter(const std::array<T, N> &coeffs) noexcept : ProcessingPolicy{ std::move(coeffs) } {}

    ~CBiquadFilter() = default;
};


#endif// !__DSP_BIQUAD_FILT_HPP__
