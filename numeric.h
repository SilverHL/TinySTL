#ifndef _NUMERIC_H_
#define _NUMERIC_H_

//计算区间内前n个元素的binary_op值
template <class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init)
{
    for ( ; first != last; first++ )
        init = init + *first;
    return init;
}
template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, 
             T init, BinaryOperation binary_op)
{
     for ( ; first != last; first++ )
        init = binary_op(init, *first);
    return init;
}

template <class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
                                   OutputIterator result)
{
    if (first == last) return result;
    *result = *first;
    return __adjacent_difference(first, last, result, value_type(first));
}


template <class InputIterator, class OutputIterator, class T>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
                                   OutputIterator result, T *)
{
    T value = *first;
    while (++first != last) 
    {
        T tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }

    return ++result;
}


template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
                                   OutputIterator result,
                                   BinaryOperation binary_op)
{
    if (first == last) return result;
    *result = *first;
    return __adjacent_difference(first, last, result, 
                                 value_type(first), binary_op);
}


template <class InputIterator, class OutputIterator, class T, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, 
                                   OutputIterator result, T *, 
                                   BinaryOperation binary_op)
{
    T value = *first;
    while (++first != last) 
    {
        T tmp = *first;
        *++result = binary_op(tmp, *value);
        value = tmp;
    }

    return ++result;
}

template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1, 
                InputIterator2 first2, T init)
{
    for ( ; first1 != last1; first1++, ++first2 )
        init = init + (*first1 * *first2);
    return init;
}


template <class InputIterator1, class InputIterator2, class T, 
          class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1, 
                InputIterator2 first2, T init, 
                BinaryOperation1 binary_op1, BinaryOperation2 binary_op2)
{
    for ( ; first1 != last1; ++first1, ++first2 )
        init = binary_op1(init , binary_op2(*first1, *first2));
    return init;
}

#endif
