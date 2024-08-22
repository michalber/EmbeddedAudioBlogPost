	.text
	.align 4

// registers with the args:     A2            A3            A4
// Call as void simd_mulacc_esp32s3(int16_t *coeffs, int16_t *delay, int32_t *acc);
	.global simd_mulacc_esp32s3
  .type   simd_mulacc_esp32s3,@function
simd_mulacc_esp32s3:
    entry                       a1,32                   # prepare windowed registers and reserve 16 bytes of stack
    movi                        a6, 0
    ee.vld.128.ip               q0, a2, 16              # load 8 coeff values into Q0 from A2, then add 16 to A2
    ee.vld.128.ip               q1, a3, 16              # load 8 delay values into Q1 from A3, then add 16 to A3
    ee.vmulas.s16.accx.ld.ip    q3, a3, 16, q0, q1
    ee.vld.128.ip               q0, a2, 16              # load 8 coeff values into Q0 from A2, then add 16 to A2
    ee.vld.128.ip               q1, a3, 16              # load 8 delay values into Q1 from A3, then add 16 to A3
    ee.vmulas.s16.accx.ld.ip    q3, a3, 16, q0, q1
    ee.srs.accx                 a4, a6, 0
	    movi.n	a2,0                                    # return value of 0
	    retw.n                                          # restore state (windowed registers) and return to caller
