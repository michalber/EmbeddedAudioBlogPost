#ifndef __UTILS_DSP_SELF_TEST_HPP__
#define __UTILS_DSP_SELF_TEST_HPP__

static double tv[8];
const int N = 3200000;

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

#define TEST(type,name,ops) void IRAM_ATTR name (void) {\
    type f0 = tv[0],f1 = tv[1],f2 = tv[2],f3 = tv[3];\
    type f4 = tv[4],f5 = tv[5],f6 = tv[6],f7 = tv[7];\
    for (int j = 0; j < N/16; j++) {\
        ops \
    }\
    tv[0] = f0;tv[1] = f1;tv[2] = f2;tv[3] = f3;\
    tv[4] = f4;tv[5] = f5;tv[6] = f6;tv[7] = f7;\
    }
    
#define fops(op1,op2) f0 op1##=f1 op2 f2;f1 op1##=f2 op2 f3;\
    f2 op1##=f3 op2 f4;f3 op1##=f4 op2 f5;\
    f4 op1##=f5 op2 f6;f5 op1##=f6 op2 f7;\
    f6 op1##=f7 op2 f0;f7 op1##=f0 op2 f1;

#define addops fops(,+) fops(,+)
#define divops fops(,/) fops(,/)
#define mulops fops(,*) fops(,*)
#define muladdops fops(+,*)


TEST(int,mulint,mulops)
TEST(float,mulfloat,mulops)
TEST(double,muldouble,mulops)
TEST(int,addint,addops)
TEST(float,addfloat,addops)
TEST(double,adddouble,addops)
TEST(int,divint,divops)
TEST(float,divfloat,divops)
TEST(double,divdouble,divops)
TEST(int,muladdint,muladdops)
TEST(float,muladdfloat,muladdops)
TEST(double,muladddouble,muladdops)

#endif // !__UTILS_DSP_SELF_TEST_HPP__