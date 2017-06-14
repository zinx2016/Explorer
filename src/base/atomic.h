#ifndef _EXPLORER_ATOMIC_H_
#define _EXPLORER_ATOMIC_H_

#include <stdint.h>

namespace Explorer {

template <typename T>
class Atomic
{
public:
        Atomic() {}

        Atomic(const T& val) : val_(val) {}

        inline T get()
        {
                return __sync_val_compare_and_swap(&val_, 0, 0);
        }

        inline T getAndAdd(T x)
        {
                return __sync_fetch_and_add(&val_, x);
        }

        inline T addAndGet(T x)
        {
                return __sync_add_and_fetch(&val_, x);
        }

        inline T getAndSub(T x)
        {
                return __sync_fetch_and_sub(&val_, x);
        }

        inline T subAndGet(T x)
        {
                return __sync_sub_and_fetch(&val_, x);
        }

        inline T operator++(int)
        {
                return getAndAdd(1);
        }

        inline T operator++()
        {
                return addAndGet(1);
        }

        inline T operator--(int)
        {
                return getAndSub(1);
        }

        inline T operator--()
        {
                return subAndGet(1);
        }

        inline Atomic<T>& operator=(const T& x)
        {
                __sync_val_compare_and_swap(&val_, val_, x);
                return *this;
        }

private:
        volatile T val_;

};

typedef Atomic<uint8_t> AtomicInt8;
typedef Atomic<uint16_t> AtomicInt16;
typedef Atomic<uint32_t> AtomicInt32;
typedef Atomic<uint64_t> AtomicInt64;

}  // namespace Explorer

#endif /*_EXPLORER_ATOMIC_H_*/
