#ifndef __DSP_FIR_FILT_HPP__
#define __DSP_FIR_FILT_HPP__

#include <array>

#include "malloc.h"

#include "dsp_common.hpp"
#include "dsp_types.hpp"
#include "qformat.hpp"

namespace dsp::fir {
typedef struct {
    double f0;
    double fs;
} kEqParams;

template<class WindowType>
struct LPF {
    template<std::size_t N, typename T = float>
    constexpr std::array<T, N> GetCoeffs(const kEqParams &params) {
        const auto            fc     = params.f0 / params.fs;
        const auto            window = WindowType::template Get<N>();
        std::array<double, N> coeffs{};
        std::array<T, N>      retCoeffs{};

        for (int i = 0; i < N; i++) { coeffs[i] = dsp::math::sinc(2.0 * fc * (i - (N - 1) / 2.0)); }

        int i = 0;
        for (auto &coeff : coeffs) { coeff *= window[i++]; }

        // Normalize to get unity gain
        auto maxAbsMultiplier = 0.0;
        for (auto &coeff : coeffs) { maxAbsMultiplier += coeff; }

        for (auto &coeff : coeffs) { coeff /= maxAbsMultiplier; }

        for (i = 0; i < N; ++i) { retCoeffs[i] = static_cast<T>(coeffs[i]); }

        return retCoeffs;
    }
};

template<class WindowType>
struct HPF {
    template<std::size_t N, typename T = float>
    constexpr std::array<T, N> GetCoeffs(const kEqParams &params) {
        const auto            fc     = params.f0 / params.fs;
        const auto            window = WindowType::template Get<N>();
        std::array<double, N> coeffs{};
        std::array<T, N>      retCoeffs{};

        for (int i = 0; i < N; i++) { coeffs[i] = dsp::math::sinc(2.0 * fc * (i - (N - 1) / 2.0)); }

        int i = 0;
        for (auto &coeff : coeffs) { coeff *= window[i++]; }

        // Normalize to get unity gain
        auto maxAbsMultiplier = 0.0;
        for (auto &coeff : coeffs) { maxAbsMultiplier += coeff; }

        for (auto &coeff : coeffs) {
            coeff /= maxAbsMultiplier;
            coeff *= -1.0;
        }
        coeffs[std::floor((N - 1) / 2)] += 1;

        for (i = 0; i < N; ++i) { retCoeffs[i] = { coeffs[i] }; }

        return retCoeffs;
    }
};

template<typename T>
class FirProcessingBasic {
  public:
    template<std::size_t N>
    FirProcessingBasic(const std::array<T, N> &&coeffs) : mN{ (int)coeffs.size() }, mDelayPos{ 0 } {
        mDelayLine = (T *)memalign(16, (mN + 4) * sizeof(T));
        for (int i = 0; i < (mN + 4); i++) { mDelayLine[i] = T{ 0 }; }
        mCoeffs = (T *)memalign(16, mN * sizeof(T));
        for (int i = 0; i < mN; i++) { mCoeffs[i] = coeffs[i]; }

        static_assert(N % 4 == 0);
    }

  public:
    inline void ProcessBlock(const T *input, T *output, uint32_t samples) {
        for (int i = 0; i < samples; ++i) {
            T   acc{ 0 };
            int coeffsPos{ 0 };
            mDelayLine[mDelayPos] = input[i];
            ++mDelayPos;
            if (mDelayPos >= mN) { mDelayPos = 0; }
            for (int n = mDelayPos; n < mN; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
            for (int n = 0; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
            output[i] = acc;
        }
    }

    inline T ProcessSample(T sample) {
        T   acc{ 0 };
        int coeffsPos{ 0 };
        mDelayLine[mDelayPos] = sample;
        ++mDelayPos;
        if (mDelayPos >= mN) { mDelayPos = 0; }
        for (int n = mDelayPos; n < mN; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
        for (int n = 0; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }

        return acc;
    }

  public:
    // __attribute__((aligned(16))) std::array<T, N> mCoeffs;
    // __attribute__((aligned(16))) std::array<T, N> mDelayLine;
    T        *mCoeffs;
    T        *mDelayLine;
    int       mDelayPos;
    const int mN;
};

namespace simd {
    template<typename T>
    class FirProcessingInnerLoopSimd {
      public:
        template<std::size_t N>
        FirProcessingInnerLoopSimd(const std::array<T, N> &&coeffs) : mN{ (int)coeffs.size() }, mDelayPos{ 0 } {
            mDelayLine = (T *)memalign(16, (mN + 4) * sizeof(T));
            for (int i = 0; i < (mN + 4); i++) { mDelayLine[i] = T{ 0 }; }
            mCoeffs = (T *)memalign(16, mN * sizeof(T));
            for (int i = 0; i < mN; i++) { mCoeffs[i] = coeffs[i]; }

            static_assert(N % 4 == 0);
        }

      public:
        inline void ProcessBlock(const T *input, T *output, uint32_t samples) {
            for (int i = 0; i < samples; ++i) {
                T   acc{ 0 };
                int coeffsPos{ 0 };
                int n;

                mDelayLine[mDelayPos++] = input[i];
                if (mDelayPos >= mN) { mDelayPos = 0; }
                for (n = mDelayPos; n < mN; n += 4) {
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
                }
                for (n = 0; n < mDelayPos; n += 4) {
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
                    acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
                }
                for (; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
                output[i] = acc;
            }
        }

        // inline T ProcessSample(T sample) {
        //     int    coeffsPos{ 0 };
        //     int    n;
        //     __m128 sum = _mm_setzero_ps();// SSE - ustawienie sumy na zero

        //     mDelayLine[mDelayPos++] = sample;
        //     if (mDelayPos >= mN) { mDelayPos = 0; }

        //     for (n = mDelayPos; n < mN; n += 4) {
        //         // Załaduj 4 próbki wejściowe (zakładając wyrównane dane)
        //         __m128 x = _mm_load_ps(&mDelayLine[n]);
        //         // Załaduj 4 wyrównane współczynniki
        //         __m128 c = _mm_load_ps(&mCoeffs[coeffsPos]);
        //         // Mnożenie wejść przez współczynniki
        //         __m128 mul = _mm_mul_ps(x, c);
        //         // Sumowanie wyników
        //         sum = _mm_add_ps(sum, mul);

        //         coeffsPos += 4;
        //     }
        //     for (n = 0; n < mDelayPos; n += 4) {
        //         // Załaduj 4 próbki wejściowe (zakładając wyrównane dane)
        //         __m128 x = _mm_load_ps(&mDelayLine[n]);
        //         // Załaduj 4 wyrównane współczynniki
        //         __m128 c = _mm_load_ps(&mCoeffs[coeffsPos]);
        //         // Mnożenie wejść przez współczynniki
        //         __m128 mul = _mm_mul_ps(x, c);
        //         // Sumowanie wyników
        //         sum = _mm_add_ps(sum, mul);

        //         coeffsPos += 4;
        //     }
        //     for (; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }

        //     // Dodawanie horyzontalne, aby uzyskać pojedynczą sumę
        //     sum = _mm_hadd_ps(sum, sum);
        //     sum = _mm_hadd_ps(sum, sum);

        //     // Zapisz wynik do zmiennej
        //     T result;
        //     _mm_store_ss(&result, sum);

        //     return result;
        // }


        inline T ProcessSample(T sample) {
            T   acc{ 0 };
            int coeffsPos{ 0 };
            mDelayLine[mDelayPos++] = sample;
            if (mDelayPos >= mN) { mDelayPos = 0; }
            int n;
            for (n = mDelayPos; n < mN; n += 4) {
                acc += mCoeffs[coeffsPos++] * mDelayLine[n];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
            }
            for (n = 0; n < mDelayPos; n += 4) {
                acc += mCoeffs[coeffsPos++] * mDelayLine[n];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
                acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
            }
            for (; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }

            return acc;
        }

      protected:
        // __attribute__((aligned(16))) std::array<T, N> mCoeffs;
        // __attribute__((aligned(16))) std::array<T, N + 4> mDelayLine;
        T        *mCoeffs;
        T        *mDelayLine;
        int       mDelayPos;
        const int mN;
    };

    template<>
    void FirProcessingInnerLoopSimd<float>::ProcessBlock(const float *input, float *output, uint32_t samples);

    template<>
    void dsp::fir::simd::FirProcessingInnerLoopSimd<QFormat<Q15>>::ProcessBlock(const QFormat<Q15> *input,
                                                                                QFormat<Q15>       *output,
                                                                                uint32_t            samples);

    // template<typename T, std::size_t N>
    // class FirProcessingOuterLoopSimd {
    //   public:
    //     FirProcessingOuterLoopSimd(const std::array<T, N> &&coeffs) : mCoeffs{ coeffs }, mDelayLine{}, mDelayPos{ 0 }
    //     {}

    //   public:
    //     void ProcessBlock(const T *input, T *output, uint32_t samples) {
    //         int      coeffsPos{ 0 };
    //         uint32_t i;
    //         uint32_t n;

    //         for (i = 0; i < samples; i += 4) {
    //             mDelayLine[(mDelayPos++) % N] = input[i + 0];
    //             mDelayLine[(mDelayPos++) % N] = input[i + 1];
    //             mDelayLine[(mDelayPos++) % N] = input[i + 2];
    //             mDelayLine[(mDelayPos++) % N] = input[i + 3];
    //             if (mDelayPos >= N) { mDelayPos = mDelayPos % N; }

    //             const auto noOf4s = (N - mDelayPos) / 4;
    //             n                 = mDelayPos;
    //             for (auto cnt = 0u; cnt < noOf4s; ++cnt) {
    //                 output[i + 0] += mDelayLine[n + 0] * mCoeffs[coeffsPos];
    //                 output[i + 1] += mDelayLine[n + 1] * mCoeffs[coeffsPos];
    //                 output[i + 2] += mDelayLine[n + 2] * mCoeffs[coeffsPos];
    //                 output[i + 3] += mDelayLine[n + 3] * mCoeffs[coeffsPos];
    //                 ++n;
    //                 ++coeffsPos;
    //             }
    //             for (; n < N; n++) { output[i] += mCoeffs[coeffsPos++] * mDelayLine[n]; }
    //             for (n = 0; n < mDelayPos; n++) {
    //                 output[i + 0] += mDelayLine[n + 0] * mCoeffs[coeffsPos];
    //                 output[i + 1] += mDelayLine[n + 1] * mCoeffs[coeffsPos];
    //                 output[i + 2] += mDelayLine[n + 2] * mCoeffs[coeffsPos];
    //                 output[i + 3] += mDelayLine[n + 3] * mCoeffs[coeffsPos];
    //                 ++coeffsPos;
    //             }
    //         }

    //         for (; i < samples; i++) { output[i] = ProcessSample(input[i]); }
    //     }

    //     inline T ProcessSample(T sample) {
    //         T   acc{ 0 };
    //         int coeffsPos{ 0 };
    //         mDelayLine[mDelayPos] = sample;
    //         ++mDelayPos;
    //         if (mDelayPos >= N) { mDelayPos = 0; }
    //         for (int n = mDelayPos; n < N; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
    //         for (int n = 0; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }

    //         return acc;
    //     }

    //   protected:
    //     alignas(16) std::array<T, N> mCoeffs;
    //     alignas(16) std::array<T, N + 4> mDelayLine;
    //     uint32_t mDelayPos;
    // };

    // template<typename T, std::size_t N>
    // class FirProcessingInnerOuterLoopSimd {
    //   public:
    //     FirProcessingInnerOuterLoopSimd(const std::array<T, N> &&coeffs)
    //         : mCoeffs{ coeffs }, mDelayLine{}, mDelayPos{ 0 } {}

    //   public:
    //     void ProcessBlock(const T *input, T *output, uint32_t samples) {
    //         int      coeffsPos{ 0 };
    //         uint32_t i;
    //         uint32_t n;

    //         mDelayLine[(mDelayPos++) % N] = input[i + 0];
    //         mDelayLine[(mDelayPos++) % N] = input[i + 1];
    //         mDelayLine[(mDelayPos++) % N] = input[i + 2];
    //         mDelayLine[(mDelayPos++) % N] = input[i + 3];

    //         if (mDelayPos >= N) { mDelayPos = mDelayPos % N; }
    //         const auto noOf4s = (N - mDelayPos) / 4;

    //         n = mDelayPos;
    //         for (auto cnt = 0u; cnt < noOf4s; ++cnt) {
    //             output[i + 0] +=
    //                 (mDelayLine[n + 0] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 3]);
    //             output[i + 1] +=
    //                 (mDelayLine[n + 1] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 3]);
    //             output[i + 2] +=
    //                 (mDelayLine[n + 2] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 3]);
    //             output[i + 3] +=
    //                 (mDelayLine[n + 3] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 3]);
    //             n += 4;
    //             coeffsPos += 4;
    //         }
    //         for (; n < N; n++) { output[i] += mCoeffs[coeffsPos++] * mDelayLine[n]; }
    //         for (n = 0; n < mDelayPos; n += 4) {
    //             output[i + 0] +=
    //                 (mDelayLine[n + 0] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 0] * mCoeffs[coeffsPos + 3]);
    //             output[i + 1] +=
    //                 (mDelayLine[n + 1] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 1] * mCoeffs[coeffsPos + 3]);
    //             output[i + 2] +=
    //                 (mDelayLine[n + 2] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 2] * mCoeffs[coeffsPos + 3]);
    //             output[i + 3] +=
    //                 (mDelayLine[n + 3] * mCoeffs[coeffsPos + 0]) + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 1])
    //                 + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 2]) + (mDelayLine[n + 3] * mCoeffs[coeffsPos + 3]);
    //             coeffsPos += 4;
    //         }

    //         for (; i < samples; i++) { output[i] = ProcessSample(input[i]); }
    //     }

    //     inline T ProcessSample(T sample) {
    //         T   acc{ 0 };
    //         int coeffsPos{ 0 };
    //         mDelayLine[mDelayPos++] = sample;
    //         if (mDelayPos >= N) { mDelayPos = 0; }
    //         const auto noOf4s = (N - mDelayPos) / 4;

    //         int n = mDelayPos;
    //         for (auto cnt = 0u; cnt < noOf4s; ++cnt) {
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
    //             n += 4;
    //         }
    //         for (; n < N; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }
    //         for (n = 0; n < mDelayPos; n += 4) {
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 1];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 2];
    //             acc += mCoeffs[coeffsPos++] * mDelayLine[n + 3];
    //         }
    //         for (; n < mDelayPos; n++) { acc += mCoeffs[coeffsPos++] * mDelayLine[n]; }

    //         return acc;
    //     }

    //   protected:
    //     alignas(16) std::array<T, N> mCoeffs;
    //     alignas(16) std::array<T, N + 4> mDelayLine;
    //     uint32_t mDelayPos;
    // };
}// namespace simd
}// namespace dsp::fir

template<typename T, class FilterType, class ProcessingPolicy = dsp::fir::FirProcessingBasic<T>>
class CFirFilter
    : public FilterType
    , public ProcessingPolicy {
  public:
    CFirFilter() = delete;

    // explicit constexpr CFirFilter(const dsp::fir::kEqParams &params)
    //     : ProcessingPolicy{ std::move(FilterType::template GetCoeffs<N, T>(params)) } {}

    template<std::size_t N>
    explicit constexpr CFirFilter(const std::array<T, N> &coeffs) noexcept : ProcessingPolicy{ std::move(coeffs) } {}

    ~CFirFilter() = default;
};

#endif// !__DSP_FIR_FILT_HPP__
