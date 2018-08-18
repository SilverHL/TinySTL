#pragma once
#ifndef ALLOC_H
#define ALLOC_H

#include <cstdlib>
#include "default_allocator.h"
#include "malloctor.h"



typedef __default_alloc_template<0, 0> default_alloc;
typedef default_alloc alloc;

template <class T, class Alloc = alloc>
    class simple_alloc
    {
    public:
        static T *allocate(size_t n) { 
            return 0 == n ? 0 : (T*)Alloc::allocate(n *sizeof(T)); 
        }

        static T *allocate(void) {
            return (T*)Alloc::allocate(sizeof(T));
        }

        static void deallocate(T *p, size_t n) {
            if (n != 0) 
                Alloc::deallocate(p, n *sizeof(T));
        }

        static void deallocate(T *p) {
            Alloc::deallocate(p, sizeof(T));
        }

    };


#endif
