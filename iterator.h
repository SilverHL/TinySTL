#pragma once
#ifndef _ITERATOR_H
#define _ITERATOR_H

#include <stdlib.h>

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public forward_iterator_tag {};


//iterator类
template <class Category, class T, class Distance = std::ptrdiff_t, 
         class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
};

//iterator traits 鉴别类型
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

//两个特化类型
//T*
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef std::ptrdiff_t              difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

//const T*
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef std::ptrdiff_t              difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};


//得到distance_type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) 
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}


//得到value_type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//不同版本的计算距离
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;

}

template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, 
           random_access_iterator_tag)
{
    return last - first;
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
    typedef typename iterator_traits<InputIterator>::iterator_category iterator_category;
    return __distance(first, last, iterator_category());
}

//不同版本的前进 或后退
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, 
                      input_iterator_tag) 
{
    while (--n) ++i;
}

template <class BidirectionalIterator, class Distance>
inline  void __advance(BidirectionalIterator& i, Distance n, 
                       bidirectional_iterator_tag) 
{
    if (n >= 0) 
        while (n--) ++i;
    else 
        while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, 
                      random_access_iterator_tag)
{
    i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) 
{
    typedef typename iterator_traits<InputIterator>::iterator_category iterator_category;
    __advance(i, n, iterator_category(i));
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type 
operator-(InputIterator first, InputIterator last) 
{
    typename iterator_traits<InputIterator>::difference_type length = 0;
    for ( ; first != last; first++ ) 
        length++;
    return length;
}


#endif
