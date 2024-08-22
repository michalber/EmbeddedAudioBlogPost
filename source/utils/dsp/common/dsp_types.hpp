#ifndef __UTILS_DSP_TYPES_HPP__
#define __UTILS_DSP_TYPES_HPP__

#include <cmath>
#include <cstdint>

// Negative 60dB in amplitude.
static constexpr float kNegative60dbInAmplitude = 0.001f;

// Tolerated error margins for floating points.
static constexpr double kEpsilonDouble = 1e-6;
static constexpr float  kEpsilonFloat  = 1e-6f;

// Inverse square root of two (equivalent to -3dB audio signal attenuation).
static const auto kInverseSqrtTwo = 1.0f / std::sqrt(2.0f);

// DSP related
static constexpr auto kNegative3dBGain = 0.7071067812f;// 1.0/sqrt(2.0)
static constexpr auto kPositive3dBGain = 1.4142135624f;// sqrt(2.0)

// Pi in radians.
static constexpr float kPi = static_cast<float>(M_PI);
// Half pi in radians.
static constexpr float kHalfPi = static_cast<float>(M_PI / 2.0);
// Two pi in radians.
static constexpr float kTwoPi = static_cast<float>(2.0 * M_PI);

static constexpr auto kNumMonoChannels   = 1;
static constexpr auto kNumStereoChannels = 2;

// Number of octave bands in which room effects are computed.
static constexpr size_t kNumReverbOctaveBands = 9;

// Centers of possible frequency bands up 8 kHz.
// ------------------------------------
// Band no.  Low     Center     High    [Frequencies in Hz]
// ------------------------------------
// 0         22        31.25     44.2
// 1         44.2      62.5      88.4
// 2         88.4      125       176.8
// 3         176.8     250       353.6
// 4         353.6     500       707.1
// 5         707.1     1000      1414.2
// 6         1414.2    2000      2828.4
// 7         2828.4    4000      5656.9
// 8         5656.9    8000      11313.7
//--------------------------------------
constexpr float kOctaveBandCentres[kNumReverbOctaveBands] = { 31.25f,  62.5f,   125.0f,  250.0f, 500.0f,
                                                              1000.0f, 2000.0f, 4000.0f, 8000.0f };

// Speed of sound in air at 20 degrees Celsius in meters per second.
// http://www.sengpielaudio.com/calculator-speedsound.htm
static constexpr float kSpeedOfSound = 343.0f;

#endif// !__UTILS_DSP_TYPES_HPP__