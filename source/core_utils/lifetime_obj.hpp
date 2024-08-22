#ifndef __CORE_UTILS_LIFETIME_OBJ_HPP__
#define __CORE_UTILS_LIFETIME_OBJ_HPP__

#include "types.h"

#include <utility>

struct LifeTObjNoDelete
{
    template<typename T>
    void operator()(T*) {}
};

using LifeTObjDeletor = LifeTObjNoDelete;

template<typename T, typename Deletor = LifeTObjDeletor>
class CLifetimeObj
{
public:
    CLifetimeObj() = default;
    CLifetimeObj(T *rObj) : mObj{rObj} {}
    ~CLifetimeObj()
    {
        Clear();
    }

    void Clear()
    {
        if(mObj)
        {
            Deletor()(mObj);
            mObj = nullptr;
        }
    }

    CLifetimeObj(CLifetimeObj &&rObj)
    {
        mObj = rObj.mObj;
        rObj.mObj = nullptr;
    }

    CLifetimeObj& operator=(CLifetimeObj &&rObj)
    {
        mObj = rObj.mObj;
        rObj.mObj = nullptr;

        return *this;
    }

public:
    T *mObj = nullptr;

    DELETE_COPY_AND_ASSIGN(CLifetimeObj)
};

template<typename T, typename Deletor = LifeTObjDeletor, typename... Args>
CLifetimeObj<T, Deletor> MakeLifetimeObj(Args&&... args)
{
    return CLifetimeObj<T, Deletor>(new T(std::forward<Args>(args)...));
}

template<typename T, typename Deletor>
T* LifetimeObjGetPtr(const CLifetimeObj<T, Deletor> &rItem)
{
    return rItem.mObj;
}

template<typename T, typename Deletor>
T& LifetimeObjGet(const CLifetimeObj<T, Deletor> &rItem)
{
    return *rItem.mObj;
}

template<typename T, typename Deletor>
void LifetimeObjClear(const CLifetimeObj<T, Deletor> &rItem)
{
    rItem.Clear();
}


#endif // !__CORE_UTILS_LIFETIME_OBJ_HPP__