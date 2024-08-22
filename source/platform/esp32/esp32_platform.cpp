#include "esp32_platform.hpp"

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include <esp_attr.h>
#include <esp_timer.h>
#include <freertos/task.h>

#include <inttypes.h>
#include <random>
#include <stdio.h>
#include <string_view>

#include "biquad_filter.hpp"
#include "dsp_self_test.hpp"
#include "dsps_biquad.h"
#include "dsps_fir.h"
#include "fir_filter.hpp"
#include "qformat.hpp"
#include "thread.hpp"
#include "timer.hpp"

namespace {
CLifetimeObj<CPlatform_ESP32::CI2S_Input>  gInI2SHandle;
CLifetimeObj<CPlatform_ESP32::CI2S_Output> gOutI2SHandle;

static void timeit(const std::string_view &name, void fn(void)) {
    vTaskDelay(1);
    tv[0] = tv[1] = tv[2] = tv[3] = tv[4] = tv[5] = tv[6] = tv[7] = 1;
    // get time since boot in microseconds
    uint64_t time = esp_timer_get_time();
    unsigned ccount, icount, ccount_new;
    RSR(CCOUNT, ccount);
    WSR(ICOUNT, 0);
    WSR(ICOUNTLEVEL, 2);
    fn();
    RSR(CCOUNT, ccount_new);
    RSR(ICOUNT, icount);
    time      = esp_timer_get_time() - time;
    float cpi = (float)(ccount_new - ccount) / icount;
    printf("%s \t %f MOP/S   \tCPI=%f\n", name.data(), (float)N / time, cpi);
}

static void RunDspTests() {
    timeit("Integer Addition", addint);
    timeit("Integer Multiply", mulint);
    timeit("Integer Division", divint);
    timeit("Integer Multiply-Add", muladdint);

    timeit("Float Addition ", addfloat);
    timeit("Float Multiply ", mulfloat);
    timeit("Float Division ", divfloat);
    timeit("Float Multiply-Add", muladdfloat);

    timeit("Double Addition", adddouble);
    timeit("Double Multiply", muldouble);
    timeit("Double Division", divdouble);
    timeit("Double Multiply-Add", muladddouble);
}

constexpr auto                                kFilterSize = 1 << 10;
constexpr auto                                kDataSize   = 1 << 10;
static constexpr __attribute__((aligned(16))) std::array<float, kFilterSize> kFilterCoeffs{
    dsp::fir::LPF<dsp::HammingWindow>{}.GetCoeffs<kFilterSize, float>(
        dsp::fir::kEqParams{ .f0 = 4000, .fs = I2S_SAMPLING_FREQ })
};

static constexpr __attribute__((aligned(16))) std::array<QFormat<Q15>, kFilterSize> kQ15FilterCoeffs{
    dsp::fir::LPF<dsp::HammingWindow>{}.GetCoeffs<kFilterSize, QFormat<Q15>>(
        dsp::fir::kEqParams{ .f0 = 4000, .fs = I2S_SAMPLING_FREQ })
};

static constexpr __attribute__((aligned(16))) std::array<float, dsp::biquad::kBiquadArraySize> kBiquadFilterCoeffs{
    dsp::biquad::LPF{}.GetCoeffs<float>(
        dsp::biquad::kEqParams{ .f0 = 4000, .fs = I2S_SAMPLING_FREQ, .gain_db = 1.0, .q = 0.707 })
};

static constexpr __attribute__((aligned(16))) std::array<QFormat<Q29>, dsp::biquad::kBiquadArraySize>
                                              kintBiquadFilterCoeffs{ dsp::biquad::LPF{}.GetCoeffs<QFormat<Q29>>(
        dsp::biquad::kEqParams{ .f0 = 4000, .fs = I2S_SAMPLING_FREQ, .gain_db = 1.0, .q = 0.707 }) };

static __attribute__((aligned(16))) float esp_data[kDataSize];
static __attribute__((aligned(16))) float esp_out[kDataSize];

static __attribute__((aligned(16))) QFormat<Q15> esp_data_int[kDataSize];
static __attribute__((aligned(16))) QFormat<Q15> esp_out_int[kDataSize];

static __attribute__((aligned(16))) QFormat<Q29> esp_data_Q29[kDataSize];
static __attribute__((aligned(16))) QFormat<Q29> esp_out_Q29[kDataSize];

template<>
void dsp::fir::simd::FirProcessingInnerLoopSimd<float>::ProcessBlock(const float *input,
                                                                     float       *output,
                                                                     uint32_t     samples) {
    fir_f32_s temp;
    temp.coeffs = this->mCoeffs;
    temp.delay  = this->mDelayLine;
    temp.N      = this->mN;
    temp.pos    = this->mDelayPos;
    dsps_fir_f32_aes3(&temp, input, output, samples);
}

extern "C" {
void simd_mulacc_esp32s3(int16_t *coeffs, int16_t *delay, int32_t *acc);
}

template<>
void dsp::fir::simd::FirProcessingInnerLoopSimd<QFormat<Q15>>::ProcessBlock(const QFormat<Q15> *input,
                                                                            QFormat<Q15>       *output,
                                                                            uint32_t            samples) {
    __attribute__((aligned(16))) int16_t temp_d[16];
    __attribute__((aligned(16))) int16_t temp_c[16];

    for (int i = 0; i < samples; ++i) {
        int coeffsPos{ 0 };

        mDelayLine[mDelayPos++] = input[i];
        if (mDelayPos >= mN) { mDelayPos = 0; }

        auto    loopLen = mN - mDelayPos;
        auto    blkCnt  = loopLen >> 4;
        int     n       = mDelayPos;
        int32_t acc     = 0;
        int32_t acc_temp;

        while (blkCnt > 0) {
            // Load 2x 8 data from coeffs and delay line
            for (int i = 0; i < 16; ++i) {
                temp_c[i] = mCoeffs[coeffsPos++].mQformatInt;
                temp_d[i] = mDelayLine[n++].mQformatInt;
            }
            // Multiply and accumulate
            simd_mulacc_esp32s3(temp_c, temp_d, &acc_temp);
            // Save to accumulator
            acc += acc_temp;
            blkCnt--;
        }
        n      = 0;
        blkCnt = mDelayPos >> 4;
        while (blkCnt > 0) {
            // Load 2x 8 data from coeffs and delay line
            for (int i = 0; i < 16; ++i) {
                temp_c[i] = mCoeffs[coeffsPos++].mQformatInt;
                temp_d[i] = mDelayLine[n++].mQformatInt;
            }
            // Multiply and accumulate
            simd_mulacc_esp32s3(temp_c, temp_d, &acc_temp);
            // Save to accumulator
            acc += acc_temp;
            blkCnt--;
        }

        // Shift accumulator
        // Save to output
        output[i].mQformatInt = acc;
    }
}

static void MeasureFirAlgorithms() {
    {
        std::generate(esp_data, esp_data + kDataSize, [&] { return rand() / INT_MAX; });

        CFirFilter<float, dsp::fir::LPF<dsp::HammingWindow>> fir{ kFilterCoeffs };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("FIR float %d: %lldus", kFilterSize, duration);
            } };
            fir.ProcessBlock(esp_data, esp_out, kDataSize);
        }
    }
    {
        std::generate(esp_data, esp_data + kDataSize, [&] { return rand(); });

        CFirFilter<float, dsp::fir::LPF<dsp::HammingWindow>, dsp::fir::simd::FirProcessingInnerLoopSimd<float>> fir{
            kFilterCoeffs
        };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("SIMD(IL) FIR float %d: %lldus", kFilterSize, duration);
            } };
            fir.ProcessBlock(esp_data, esp_out, kDataSize);
        }
    }
    {
        std::generate(esp_data_int, esp_data_int + kDataSize, [&] { return QFormat<Q15>(rand()); });

        CFirFilter<QFormat<Q15>, dsp::fir::LPF<dsp::HammingWindow>> fir{ kQ15FilterCoeffs };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("FIR Q15 %d: %lldus", kFilterSize, duration);
            } };
            fir.ProcessBlock(esp_data_int, esp_out_int, kDataSize);
        }
    }
    {
        std::generate(esp_data_int, esp_data_int + kDataSize, [&] { return QFormat<Q15>(rand()); });

        CFirFilter<QFormat<Q15>,
                   dsp::fir::LPF<dsp::HammingWindow>,
                   dsp::fir::simd::FirProcessingInnerLoopSimd<QFormat<Q15>>>
            fir{ kQ15FilterCoeffs };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("SIMD(IL) FIR Q15 %d: %lldus", kFilterSize, duration);
            } };
            fir.ProcessBlock(esp_data_int, esp_out_int, kDataSize);
        }
    }
}

static void MeasureIirAlgorithms() {
    {
        std::generate(esp_data, esp_data + kDataSize, [&] { return rand() / INT_MAX; });

        {
            float         w[2]{};
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("Biquad ESP32-S3 float %d: %lldus", kFilterSize, duration);
            } };
            dsps_biquad_f32_aes3(esp_data, esp_out, kDataSize, (float *)kBiquadFilterCoeffs.data(), &w[0]);
        }
    }
    {
        std::generate(esp_data, esp_data + kDataSize, [&] { return rand() / INT_MAX; });

        CBiquadFilter<float, dsp::biquad::LPF> biquad{ kBiquadFilterCoeffs };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("Biquad float %d: %lldus", kFilterSize, duration);
            } };
            biquad.ProcessBlock(esp_data, esp_out, kDataSize);
        }
    }
    {
        std::generate(esp_data_Q29, esp_data_Q29 + kDataSize, [&] { return QFormat<Q29>(rand()); });

        CBiquadFilter<QFormat<Q29>, dsp::biquad::LPF> biquad{ kintBiquadFilterCoeffs };
        {
            CMeasureTimer timer{ [](const int64_t &startTime, const int64_t &endTime, const int64_t &duration) {
                CORE_LOG_INFO("Biquad QFormat<Q29> %d: %lldus", kFilterSize, duration);
            } };
            biquad.ProcessBlock(esp_data_Q29, esp_out_Q29, kDataSize);
        }
    }
}

static void MeasureDspAlgorithms() {
    MeasureFirAlgorithms();
    MeasureIirAlgorithms();
}

static void AppTask(void *ctx) { MeasureDspAlgorithms(); }
}// namespace


CPlatform_ESP32::CPlatform_ESP32() : mAudioPipeline{ { gInI2SHandle }, {}, {}, { gOutI2SHandle } } {}

CPlatform_ESP32::~CPlatform_ESP32() {}

void CPlatform::Assert(const char *rpMsg, const char *rpFileName, int rdLine) {}

void CPlatform::LaunchPlatform() {
    CPlatform *platform = new CPlatform_ESP32;
    (void)platform;
}

void CPlatform_ESP32::InitDrivers() {
    gInI2SHandle  = MakeLifetimeObj<CI2S_Input>(I2S_IN_BCLK_GPIO, I2S_IN_WS_GPIO, I2S_IN_DATA_GPIO);
    gOutI2SHandle = MakeLifetimeObj<CI2S_Output>(I2S_OUT_BCLK_GPIO, I2S_OUT_WS_GPIO, I2S_OUT_DATA_GPIO);
}

void CPlatform_ESP32::InitPlatform() {
    RunDspTests();
    CThread appTask{ AppTask, nullptr, "AppTask", 3, 1024 * 8 };
    appTask.Start();
}
