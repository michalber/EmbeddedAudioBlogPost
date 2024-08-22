#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef EXTERN_CPP_BEGIN
#define EXTERN_CPP_BEGIN                                                                                               \
    #ifdef __cplusplus extern "C" {                                                                                    \
        #endif
#endif// !EXTERN_CPP_BEGIN

#ifndef EXTERN_CPP_END
#define EXTERN_CPP_END                                                                                                 \
    #ifdef __cplusplus                                                                                                 \
    }                                                                                                                  \
    #endif
#endif// !EXTERN_CPP_END

#define DELETE_COPY_AND_ASSIGN(classname)                                                                              \
    classname(classname &)                  = delete;                                                                  \
    classname(const classname &)            = delete;                                                                  \
    classname &operator=(classname &)       = delete;                                                                  \
    classname &operator=(const classname &) = delete;


#define NO_COPY_OR_ASSIGN(classname)                                                                                   \
  private:                                                                                                             \
    classname(const classname &);                                                                                      \
    classname &operator=(const classname &x) { return (classname &)x; }

#define DEFAULT_MOVE(classname)                                                                                        \
  public:                                                                                                              \
    classname(classname &&) = default;

#ifdef __cplusplus
/**
 * @brief Represent Hz value in scalar
 *
 */
constexpr long long unsigned operator"" _Hz(long long unsigned hz) { return hz; }

/**
 * @brief Represent kHz value in scalar
 *
 */
constexpr long long unsigned operator"" _kHz(long double hz) { return hz * 1000; }
#endif

#endif// !__TYPES_H__