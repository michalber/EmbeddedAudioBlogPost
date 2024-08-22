#ifndef __CORE_UTILS_SINGLETON__
#define __CORE_UTILS_SINGLETON__

#include <type_traits>

namespace core {
template<typename Derived>
class Singleton {
  public:
    static Derived *GetInstance() {
        return Singleton<Derived>::mInstance;
    }

  protected:
    Singleton(Derived *pDerived) { Singleton<Derived>::mInstance = pDerived; }

    Singleton(const Singleton &)            = delete;
    Singleton(Singleton &&)                 = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton &operator=(Singleton &&)      = delete;
    ~Singleton()                            = default;

  public:
    static inline Derived *mInstance = nullptr;
};

}// namespace core


#endif// !__CORE_UTILS_SINGLETON__