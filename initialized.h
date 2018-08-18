#pragma once
#ifndef _INITIALIZED_H
#define _INITIALIZED_H

#include "construct.h"
#include <cstring>
#include "type_traits.h"
#include <algorithm>

//uninitialized_fill_n
template <class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, 
                           const T& x, __true_type) 
{
    return fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, 
                           const T& x, __false_type)
{
    ForwardIterator cur = first;
    for ( ; n > 0; --n, ++cur, ++first ) {
        construct(&*cur, first);          
    }
    return cur;
}
    
template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
    typedef typename __type_traits<T1>::is_POD_type is_POS;
    return __uninitialized_fill_n_aux(first, n, x, is_POS());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitalized_fill_n(ForwardIterator first, Size n, const T& x) 
{
    return __uninitialized_fill_n(first, n, x, value_type(first));
}

//uninitialized_copy
template <class InputIterator, class ForwardIterator>
inline ForwardIterator 
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
                                ForwardIterator result, __true_type)
{
    return std::copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __false_type)
{
    InputIterator cur = first;
    for ( ; first != last; first++, cur++) {
        construct(&*cur, *first);
    }

    return result;
}

inline char* 
uninitialized_copy(const char* first, const char* last, 
                                char *result)
{
   memmove(result, first, last - first);
   return result + (last - first );
}

inline wchar_t *
uninitialized_copy(const wchar_t *first, const wchar_t *last, 
                   wchar_t *result)
{
    memmove(result, first, last - first);
    return result + (last - first);
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, 
                     ForwardIterator result, T*) 
{
    typedef typename __type_traits<T>::is_POD_TYPE is_POD;
    return __uninitialized_copy_aux(first, last, result, is_POD());
}


template <class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, 
                   ForwardIterator result)
{
    return __uninitialized_copy(first, last, result, value_type(result));
}

//uninitialized_fill
template <class InputIterator, class T>
inline void 
__uninitialized_fill_aux(InputIterator first, InputIterator last, const T& x, __true_type)
{
    fill(first, last, x);
}

template <class InputIterator, class T>
inline void
__uninitialized_fill_aux(InputIterator first, InputIterator last, const T& x, __false_type)
{
    InputIterator cur = first;
    for ( ; first != last; ++cur, ++first ) {
       construct(&*cur, x); 
    }
}

template <class InputIterator, class T, class T1>
inline void 
__uninitialized_fill(InputIterator first, InputIterator last, const T& x, T1 *)
{
     typedef typename __type_traits<T1>::iS_POD_TYPE is_POD;
     __uninitialized_fill_aux(first, last, x, is_POD());
}


template <class InputIterator, class T>
inline void
uninitialized_fill(InputIterator first, InputIterator last, const T& value)
{
    __uninitialized_fill(first, last, value, value_type(value));
}



#endif
