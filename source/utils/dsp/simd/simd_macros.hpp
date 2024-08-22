#ifndef __UTILS_SIMD_MACROS_HPP__
#define __UTILS_SIMD_MACROS_HPP__

#if !defined(DISABLE_SIMD) && (defined(__aarch64__) || (defined(__arm__) && defined(__ARM_NEON__)))
// ARM NEON is enabled.
#include <arm_neon.h>
typedef float32x4_t SimdVector;
#define SIMD_NEON
#define SIMD_LENGTH 4
#define SIMD_MULTIPLY(a, b) vmulq_f32(a, b)
#define SIMD_ADD(a, b) vaddq_f32(a, b)
#define SIMD_SUB(a, b) vsubq_f32(a, b)
#define SIMD_MULTIPLY_ADD(a, b, c) vmlaq_f32(c, a, b)
#define SIMD_SQRT(a) vrecpeq_f32(vrsqrteq_f32(a))
#define SIMD_RECIPROCAL_SQRT(a) vrsqrteq_f32(a)
#define SIMD_LOAD_ONE_FLOAT(p) vld1q_dup_f32(&(p))
#elif !defined(DISABLE_SIMD) && (defined(TARGET_ESP32S3)))

int esp32s3_addf32x4(float *pA, float *pB) {
    asm("entry   a1,16"                          // prepare windowed registers and reserve 16 bytes of stack
        "EE.LDF.128.IP f3, f2, f1, f0, a2, 16"  // Load coeffs
        "EE.LDF.128.IP f11, f10, f9, f8, a3, 16"// Load data from delay line
        "madd.s  f4, f0, f8"
        "madd.s  f5, f1, f9"
        "madd.s  f6, f2, f10"
        "madd.s  f7, f3, f11"
        "ssip     f4, a4, 4"
        "ssip     f5, a4, 4"
        "ssip     f6, a4, 4"
        "ssip     f7, a4, 4"
        "movi.n	a2,0"           // return value of 0
        "retw.n"                // restore state (windowed registers) and return to caller
    );
}

typedef __attribute__((aligned(16))) float[4] SimdVector;
#define SIMD_ESP32S3
#define SIMD_LENGTH 4
#define SIMD_MULTIPLY(a, b) vmulq_f32(a, b)
#define SIMD_ADD(a, b)
#define SIMD_SUB(a, b) vsubq_f32(a, b)
#define SIMD_MULTIPLY_ADD(a, b, c) vmlaq_f32(c, a, b)
#define SIMD_SQRT(a) vrecpeq_f32(vrsqrteq_f32(a))
#define SIMD_RECIPROCAL_SQRT(a) vrsqrteq_f32(a)
#define SIMD_LOAD_ONE_FLOAT(p) vld1q_dup_f32(&(p))
#else
// No SIMD optimizations enabled.
// #include "base/misc_math.h"
typedef float SimdVector;
#define SIMD_DISABLED
#define SIMD_LENGTH 1
#define SIMD_MULTIPLY(a, b) ((a) * (b))
#define SIMD_ADD(a, b) ((a) + (b))
#define SIMD_SUB(a, b) ((a) - (b))
#define SIMD_MULTIPLY_ADD(a, b, c) ((a) * (b) + (c))
#define SIMD_SQRT(a) (1.0f / FastReciprocalSqrt(a))
#define SIMD_RECIPROCAL_SQRT(a) FastReciprocalSqrt(a)
#define SIMD_LOAD_ONE_FLOAT(p) (p)
#warning "Not using SIMD optimizations!"
#endif

#endif// !__UTILS_SIMD_MACROS_HPP__