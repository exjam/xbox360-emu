#ifndef PPC_UTIL_H
#define PPC_UTIL_H

namespace ppc
{

template<int N>
static inline int64_t SignExtend(uint64_t v)
{
   return static_cast<int64_t>((v & (1 << (N - 1))) ? (v | ~((1ull << N) - 1)) : v);
}

template<typename T>
static inline T Bit(T value, uint32_t bit)
{
   return (value >> bit) & 1;
}

template<int width>
static inline uint32_t BitBe(uint32_t value, uint32_t bit)
{
   return (value >> (width - 1 - bit)) & 1;
}

template<int width>
static inline uint64_t BitBe(uint64_t value, uint32_t bit)
{
   return (value >> (width - 1 - bit)) & 1;
}

}; // namespace ppc

#endif // PPC_UTIL_H
