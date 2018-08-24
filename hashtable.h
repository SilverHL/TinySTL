#pragma once
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "iterator.h"
#include "allocate.h"
#include "exception.h"
#include <vector>

using std::vector;
using std::pair;

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
    typedef Value       value_type;
    typedef Key         key_type;
    typedef HashFcn     hasher;
    typedef EqualKey    key_equal;
    typedef size_t      size_type;

private:
    hasher      hash;
    key_equal   equals;
    ExtractKey  get_key;

    typedef __hashtable_node<Value> node;
    typedef simple_alloc<node, Alloc> node_allocator;

    typedef __hashtable_iterator<Value, Key, HashFcn, 
                    ExtractKey, EqualKey, Alloc>
                        iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, 
                    ExtractKey, EqualKey, Alloc>
                        const_iterator;

    vector<node*, Alloc> buckets;
    size_type num_elements;

    size_type next_size(size_type n)
        { return __stl_next_prime(n); }
public:
    //bucket node操作相关
    //bucket 个数 即vector大小
    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const 
        { return __stl_prime_list[__stl_num_primes - 1]; }

    node* new_node(const value_type& obj)
    {
        node* n = node_allocator::allocator();
        n->next = 0;
        __STL_TRY 
        {
            construct(&n->val, obj);
            return n;
        }

        __STL_UNWIND(node_allocator::deallocator(n));
    }

    void delete_node(node* n)
    {
        destroy(&n->val);
        node_allocator::deallocate(n);
    }
    
    void initialize_buckets(size_type n)
    {
        const size_type n_buckets = next_size(n);
        buckets.reverve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)0);
        num_elements = 0;
    }

    void resize(size_type num_elements_hint);
    void copy_from(const hashtable_&);
    void clear();

    size_type btk_num(const value_type& obj, size_t n) const
    { return btk_num_key(get_key(obj), n); }
    size_type btk_num(const value_type& obj) const
    { return btk_num_key(get_key(obj)); }
    
    size_type btk_num_key(const key_type& key) const
    { return btk_num_key(key, buckets.size()); }
    size_type btk_num_key(const key_type& key, size_t n) const
    { return hash(key) % n; }
public:
    //构造函数
    hashtable_(size_type n, 
              const HashFcn& hf, 
              const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(n)
    {
        initialize_buckets(n);
    }

public:
    pair<iterator, bool> insert_unique(const value_type& obj)
    {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    iterator insert_equal(const value_type& obj)
    {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    iterator insert_equal_noresize(const value_type& obj);

    iterator find(const key_type& x) const
    {
        size_type n = bkt_num_key(x);
        node *first;

        for (first = buckets[n]; 
             first && !equals(get_key(first->val), x); 
             first = first->next);
        return iterator(first, this);
    }

    size_type count(const key_type& x) const
    {
        const size_type n = btk_num_key(x);
        size_type result = 0;

        for (const node* cur = buckets[n]; cur; cur++)
            if (equals(get_key(cur->val), x))
                result++;

        return result;
    }
};

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable_<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint)
{
    //判定是否要重新配置vector buckets的标准是
    //看hashtable内部的元素有没有大于vector的长度 
    //目测是为了 减少拉链长度 提升查找效率
    const size_type old_num = buckets.size();
    if (old_num < num_elements_hint)
    {
        const size_type new_bucket_size = next_size(num_elements_hint);
        if (new_bucket_size > old_num)
        {
            vector<node*, A> tmp(new_bucket_size, (node*)new_bucket_size);
            __STL_TRY
            {
                //单个处理每个bucket
                for (size_type bucket = 0; bucket < old_num; ++bucket)
                {
                    node* first = buckets[bucket];
                    while (first)
                    {
                        size_type new_bucket =      /* 找出当前点会落在新bucket的位置*/
                            bkt_num(first->val, new_bucket_size);
                        buckets[bucket] = first->next;  /*将当前old_bucket指向下一个元素*/
                        first->next = tmp[new_bucket]; /*将当前点插入到new_bucket的第一个元素*/
                        tmp[new_bucket] = first; /*此时first成为new_bucket的第一个元素*/
                        first = buckets[bucket]; /*指向原bucket 继续处理*/
                    }
                }

                buckets.swap(tmp);
            }
        }
    }

}

/***
 * 在不需要重建bucket的时候插入元素 
 * **/
template <class V, class K, class HF, class Ex, class Eq, class A>
pair<typename hashtable_<V, K, HF, Ex, Eq, A>::iterator, bool>
hashtable_<V, K, HF, Ex, Eq, A>::insert_unique_noresize(const value_type& obj)
{
    const size_type n = btk_num(obj);   //obj处于n
    node* first = buckets[n];           //first指向buckets的第一个位置

    for (node *cur = first; cur; cur->next)
    {
        if (equals(get_key(cur->val), get_key(obj)))
            return pair<iterator, bool>(iterator(cur, this), false);

    }
    
    node* tmp = new_node(obj);
    tmp->next = buckets[n];
    buckets[n] = tmp;
    ++num_elements;
    return pair<iterator, bool>(iterator(tmp, this), true);
}

template <class V, class K, class HF, class Ex, class Eq, class A>
typename hashtable_<V, K, HF, Ex, Eq, A>::iterator
hashtable_<V, K, HF, Ex, Eq, A>::insert_equal_noresize(const value_type& obj)
{
    const size_type n = btk_num(obj);
    node* first = buckets[n];

    //如果buckets[n]不为0 说明已经有链表
    for (node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur), get_key(obj)))
        {
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }

    node* tmp = new_node(obj);
    tmp->next = buckets[n];
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);
}

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable_<V, K, HF, Ex, Eq, A>::clear()
{
    for (size_type i = 0; i < buckets.size(); i++) 
    {
        node* cur = buckets[i];
        while (cur != 0)
        {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = 0;
    }
    num_elements = 0;
}

template <class V, class K, class HF, class Ex, class Eq, class A>
void hashtable_<V, K, HF, Ex, Eq, A>::copy_from(const hashtable_& ht)
{
    buckets.clear();
    buckets.reserve(ht.buckets.size());

    buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
    __STL_TRY
    {
        for (size_type i = 0; i < ht.buckets.size(); i++)
        {
            if (const node* cur = ht.buckets[i])
            {
                node* copy = new_node(cur->val);
                buckets[i] = copy;

                for (node* next = cur->next; next; next = cur->next)
                {
                    copy->next = new_node(next->val);
                    copy = copy->next;
                }
            }
        }
        num_elements += ht.num_elements;
    }
    __STL_UNWIND(clear());
}

#endif
