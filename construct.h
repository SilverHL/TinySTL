#pragma once
#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <new>
#include "./type_traits.h"
#include "iterator.h"

template <class T1, class T2>
inline void construct(T1* p, const T2& value)
{
    new(p) T1(value); //placement new 调用T1::T1(value)
}

//destroy第一个版本 接受一个指针
template <class T>
inline void destroy(T* pointer)
{
    pointer->~T();
}
//判断元素的value type 是否有trivial destrutor
template <class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

//通过type_traits得出的destructor type调用以下两个函数 如果是trivial的 那么就自行析构
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
    for ( ; first < last; first++)
        destroy(&*first);
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

//destroy第二个版本 接受两个迭代器 设法找出元素的数值型别
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
    __destroy(first, last, value_type(first));
}


inline void destroy(char *, char*) {}
inline void destroy(wchar_t *, wchar_t *) {}


#endif
