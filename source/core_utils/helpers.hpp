#ifndef __CORE_UTILS_HELPERS_HPP__
#define __CORE_UTILS_HELPERS_HPP__

#include <cstddef>

template<typename T>
void CopyMonoBufferToInterleaved(const T *in, T *out, std::size_t N)
{
    for(std::size_t i = 0; i < N; ++i)
    {
        (*(out++)) = (*in);
        (*(out++)) = (*in);
        ++in;
    }
}

template<typename T>
void CopyInterleavedToMonoBuffer(const T *in, T *out, std::size_t N)
{
    for(std::size_t i = 0; i < N; ++i)
    {
        (*out++) = (*(in++));
        ++in;
    }
}

#endif // !__CORE_UTILS_HELPERS_HPP__