#ifndef _SET_H_
#define _SET_H_

#include "rb_tree.h"
#include "allocate.h"
#include "iterator.h"

template <class Key, 
          class Compare = std::less<Key>, 
          class Alloc = alloc>
class set
{
public:
    typedef Key     key_type;
    typedef Key     value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;

private:
    typedef rb_tree<key_type, value_type, 
                    std::_Identity<value_type>, key_compare, Alloc> rep_type;
    rep_type t;

public:
    typedef typename rep_type::const_pointer    pointer;
    typedef typename rep_type::const_pointer    const_pointer;
    typedef typename rep_type::const_reference  reference;
    typedef typename rep_type::const_reference  const_reference;
    typedef typename rep_type::const_iterator   iterator;
    typedef typename rep_type::const_iterator   const_iterator;
    typedef typename rep_type::size_type        size_type;
    typedef typename rep_type::difference_type  difference_type;

    //set use insert_unique 
    //multiset use inert_equal
    set() : t(Compare()) {}
    explicit set(const Compare& comp) : t(comp) {}

    template <class InputIterator>
        set(InputIterator first, InputIterator last) 
        : t(Compare()) { t.insert_unique(first, last); }

    template <class InputIterator>
        set(InputIterator first, InputIterator last, const Compare& comp)
        : t(comp) { t.insert_unique(first, last); }

    set(const set<Key, Compare, Alloc>& x) : t(x.t) {}
    set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>&x) 
    {
        t = x.t;
        return *this;
    }

    key_compare key_comp() const { return t.key_comp(); }
    value_compare value_comp() const { return t.key_comp; }
    iterator begin() const { return t.begin(); }
    iterator end() const { return t.end(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }
};

#endif
