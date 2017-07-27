#ifndef _EXPLORER_ATOMIC_H_
#define _EXPLORER_ATOMIC_H_

#include <stdint.h>

namespace Explorer {

template <typename T>
class Atomic
{
public:
        Atomic() : val_(0) {}

        Atomic(const T& val) : val_(val) {}

        Atomic(const Atomic<T>& x)
        {
                __sync_val_compare_and_swap(&val_, val_, x.val_);
        }

        T Get()
        {
                return __sync_val_compare_and_swap(&val_, 0, 0);
        }

        T GetAndAdd(T x)
        {
                return __sync_fetch_and_add(&val_, x);
        }

        T AddAndGet(T x)
        {
                return __sync_add_and_fetch(&val_, x);
        }

        T GetAndSub(T x)
        {
                return __sync_fetch_and_sub(&val_, x);
        }

        T SubAndGet(T x)
        {
                return __sync_sub_and_fetch(&val_, x);
        }

        T operator++(int)
        {
                return GetAndAdd(1);
        }

        T operator++()
        {
                return AddAndGet(1);
        }

        T operator--(int)
        {
                return GetAndSub(1);
        }

        T operator--()
        {
                return SubAndGet(1);
        }

        Atomic<T>& operator=(const T& x)
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
