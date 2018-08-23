#pragma once
#ifndef _MAP_H_
#define _MAP_H_

#include "rb_tree.h"
#include "allocate.h"

template <class Key, class T, 
          class Compare = std::less<Key>, 
          class Alloc = alloc>
class map
{
public:
    
    //typedefs
    typedef Key                     key_type;
    typedef T                       data_type;
    typedef T                       mapper_data;
    typedef Compare                 key_compare;
    typedef std::pair<const Key, T> value_type;

    //一个functor 用于调用元素比较函数
    class value_compare
        : public std::binary_function<value_type, value_type, bool> 
    {
        friend class map<Key, T, Compare, Alloc>;

    protected:
        Compare comp;
        value_compare(Compare c) : comp(c) {}

    public:
        bool operator()(const value_type& x, const value_type& y) const
            { return comp(x.first, y.first); }
    };

private:
    //以下定义表述型别
    //以map元素的型别的第一型别 作为红黑树节点的键值型别
    typedef rb_tree<key_type, value_type, 
                    std::_Select1st<value_type>, key_compare, Alloc>rep_type;    
    rep_type t;

public:
    typedef typename rep_type::pointer          pointer;
    typedef typename rep_type::const_pointer    const_pointer;
    typedef typename rep_type::reference        reference;
    typedef typename rep_type::const_reference  const_reference;
    typedef typename rep_type::iterator         iterator;
    typedef typename rep_type::const_iterator   const_iterator;
    typedef typename rep_type::size_type        size_type;
    typedef typename rep_type::difference_type  difference_type;

    map() : t(Compare()) {}
    explicit map(const Compare& comp) : t(comp) {}

    template <class InputIterator>
    map(InputIterator first, InputIterator last) 
        : t(Compare()) { t.insert_unique(first, last); }

    template <class InputIterator>
    map(InputIterator first, InputIterator last, const Compare& c)
        : t(c) { t.insert_unique(first, last); }

    map(const map<Key, T, Compare, Alloc>& x) : t(x.t) {}
    map<Key, T, Compare, Alloc>& operator=(const map<Key, T, Compare, Alloc>& x)
    {
        t = x.t;
        return *this;
    }   

    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return value_compare(t.key_comp()); }
    iterator begin() { return t.begin(); }
    const_iterator begin() const { return t.begin(); }
    iterator end() { return t.end(); }
    const_iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }

    T& operator[] (const key_type& k)
    {
        return (*((insert(value_type(k, T()))).first)).second;
    }

    void swap(map<Key, T, Compare, Alloc>& x) { t.swap(x.t); }
    
    std::pair<iterator, bool> insert(const value_type& x) 
    { return t.insert_unique(x); }

    iterator insert(iterator position, const value_type& x)
    { return t.insert_unique(position, x); }

    template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { t.insert_unique(first, last); }

    void erase(iterator position) { t.erase(position); }
    size_type erase(const key_type&x) { return t.erase(x); }
    void erase(iterator first, iterator last) { t.erase(first, last); }
    void clear() { t.clear(); }

    iterator find(const key_type& x) { return t.find(x); }
    const_iterator find(const key_type& x) const { return t.find(x); }
    size_type count(const key_type& x) const { return t.count(x); }

    iterator lower_bound(const key_type& x) { return t.lower_bound(x); }
    const_iterator lower_bound(const key_type& x) const { return t.lower_bound(x); }

    iterator upper_bound(const key_type& x) { return t.upper_bound(x); }
    const_iterator upper_bound(const key_type& x) const { return t.upper_bound(x); }

    std::pair<iterator, iterator> equal_range(const key_type& x) 
        { return t.equal_range(x); }
    std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const
        { return t.equal_range(x); }

    friend bool operator== (const map&, const map&);
    friend bool operator<  (const map&, const map&);

};

template <class Key, class T, class Compare, class Alloc>
inline bool operator==(const map<Key, T, Compare, Alloc>& x, 
                       const map<Key, T, Compare, Alloc>& y)
{ return x.t == y.t; }

template <class Key, class T, class Compare, class Alloc>
inline bool operator<(const map<Key, T, Compare, Alloc>& x, 
                      const map<Key, T, Compare, Alloc>& y)
{
    return x.t < y.t;
}
#endif
