#pragma once
#ifndef _HEAP_H
#define _HEAP_H

template <class RandomAccessIterator>
inline void pop_head(RandomAccessIterator first, RandomAccessIterator last)
{
    __pop_head_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __pop_head_aux(RandomAccessIterator first, 
                           RandomAccessIterator last, 
                           T*)
{
   __pop_heap(first, last-1, T(*(last-1)), 
              distance_type(first));  
}

template <class RandomAccessIterator, class T, class Distance>
inline void __pop_head(RandomAccessIterator first, 
                       RandomAccessIterator last,
                       RandomAccessIterator result,
                       T* value, Distance*)
{
    *result = *first;
    __adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, 
                   Distance len, T value)
{
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;
    while (secondChild < len)
    {
        if (*(first + secondChild) < *(first + secondChild - 1))
            secondChild--;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);
    }
}

template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first,
                 RandomAccessIterator last, 
                 T *, Distance *)
{
    if (last - first < 2 )
        return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;

    while (true)
    {
        __adjust_heap(first, parent, len, T(*(first + parent)));
        if (parent == 0)
            break;
        parent--;
    }
}
#endif



