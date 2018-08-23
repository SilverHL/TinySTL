#pragma once
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "iterator.h"
#include "allocate.h"
#include <vector>

using std::vector;

template <class Value, class Key, class HashFcn, 
          class ExtractKey, class EqualKey, class Alloc> 
class hashtable_;

template <class Value, class Key, class HashFcn, 
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator;

template <class Value, class Key, class HashFcn, 
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

static const int __stl_num_primes = 15;
static const unsigned long __stl_prime_list[__stl_num_primes] = 
{
    53,         97,         193,        389,        769,
    1543,       3079,       6151,       12289,      24593,
    49157,      98317,      196613,     393241,     786433
};

inline unsigned long __stl_next_prime(unsigned long n)
{
    const unsigned long* first = __stl_prime_list;
    const unsigned long* last = __stl_prime_list + __stl_num_primes;
    const unsigned long* pos = std::lower_bound(first, last, n);

    return pos == last ? *(last - 1) : *pos;
}

template <class Value>
struct __hashtable_node
{
    __hashtable_node* next;
    Value val;
};


template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator 
{
    typedef hashtable_<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, 
                                 ExtractKey, EqualKey, Alloc>
            iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, 
                                 ExtractKey, EqualKey, Alloc>
            const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur;
    hashtable* ht;

    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
    __hashtable_iterator() {}

    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++();
    iterator operator++(int);

    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
};

template <class Value, class Key, class HashFcn, 
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator
{
    typedef hashtable_<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, 
            ExtractKey, EqualKey, Alloc>
                const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef const Value&            reference;
    typedef const Value*            pointer;

    const node* cur;
    const hashtable* ht;

    __hashtable_const_iterator(node* n, hashtable* tab) :
        cur(n), ht(tab) {}

    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++();
    const_iterator operator++(int);

    bool operator==(const const_iterator& x) 
    { return cur == x.cur; }
    bool operator!=(const const_iterator& x)
    { return cur != x.cur; }

};
template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>&
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++()
{
    const node* old = cur;
    cur = cur->next;
    if (!cur) {
        size_type bucket = ht->bkt_num(old->val);
        while (!cur && ++bucket < ht->buckets.size()) 
            cur = ht->buckets[bucket];
    }
    return *this;
}

template <class V, class K, class HF, class ExK, class EqK, class A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>
__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int)
{
    iterator temp = *this;
    ++*this;
    return temp;
}

template <class Value, class Key, class HashFcn, 
          class ExtractKey, class EqualKey, class Alloc>
class hashtable_
{
public:
    typedef HashFcn     hasher;
    typedef EqualKey    key_equal;
    typedef size_t      size_type;

private:
    hasher      hash;
    key_equal   equals;
    ExtractKey  get_key;

    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    vector<node*, Alloc> buckets;
};
#endif
