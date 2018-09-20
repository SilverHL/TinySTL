#pragma once
#ifndef _LIST_H
#define _LIST_H

#include "allocate.h"
#include "./type_traits.h"
#include "iterator.h"
#include <cstdlib>


template <class T>
    struct __list_node 
{
    typedef void * void_pointer;
    void_pointer next;
    void_pointer prev;
    T data;
};

template <class T, class Ref, class Ptr>
class __list_iterator 
{
public:
    typedef __list_iterator<T, T&, T*>      iterator;
    typedef __list_iterator<T, Ref, Ptr>    self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T               value_type;
    typedef Ptr             pointer;
    typedef Ref             reference;
    typedef __list_node<T>* link_type;
    typedef size_t          size_type;
    typedef std::ptrdiff_t       difference_type;

    link_type node; //list迭代器内部指针 指向list节点

    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    bool operator== (const self& x) const { return node == x.node; }
    bool operator!= (const self& x) const { return node != x.node; }

    reference operator* () const { return (*node).data; }

    pointer operator-> () const { return &operator*(); }

    //对迭代器+1就是前进一个节点
    //所谓前++就是调用之前+1
    //在这里表现为 将本link_type node +1
    //并返回+1后的值
    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }

    //后++表现为 先调用某对象然后+1 
    //在这里为先将原来的对象保存
    //然后调用前++ 将本身+1
    //最后返回原来的对象
    self& operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        node = (link_type)(*node).prev;
        return *this;
    }

    self& operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

};

template <class T, class Alloc = alloc>
class list 
{
public:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node, Alloc> list_node_allocator;
    typedef __list_iterator<T, T&, T*> iterator;

    typedef T                   value_type;
    typedef T&                  reference;
    typedef T*                  pointer;
    typedef std::ptrdiff_t      size_type;

    typedef list_node* link_type;

protected:
    //根据这个node节点来表示整个list
    //该node指向最后一个空节点 
    //以此表示list的前闭后开区间
    link_type node;
    link_type get_node() { return list_node_allocator::allocate(); }
    link_type put_node(link_type p) { list_node_allocator::deallocate(p); }

    //产生一个元素
    link_type create_node(const T& x) {
        link_type p = get_node();
        construct(&p->data, x);
        return p;
    }

    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }

public:

    iterator begin() { return (link_type)((*node).next); }
    iterator end() { return node; }

    bool empty() const { return node->next == node; }
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }

    reference front() { return *begin(); }
    reference back() { return *(--end()); }

    list() { empty_inisialize(); }

    void splice(iterator position, list& x) {
        if (x.empty()) {
            transfer(position, x.begin(), x.end());
        }
    }

    void splice(iterator position, list&, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) 
            return;
        transfer(position, i, j);
    }

    void splice(iterator position, list&, iterator first, iterator last) {
        if (first != last) 
            transfer(position, first, last);
    }

    void clear();
    void remove(const T&);
    void unique();

    void merge(list<T, Alloc>&);
    void reverse();
    void sort();

    //插入点为position之前 
    iterator insert(iterator position, const T& x) {
        link_type temp = create_node(x);
        temp->next = position.node;
        temp->prev = position.node->prev;
        (link_type(position.node->prev))->next = temp;
        position.node->prev = temp;
        return temp; 
    }

    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }

    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);

        prev_node->next = next_node;
        next_node->prev = prev_node;

        destroy_node(position.node);
        return iterator(next_node);
    }

    void pop_front() { erase(begin()); }
    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

    void swap(list<T, Alloc>& x) { std::swap(node, x.node); }
protected:
    void empty_inisialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }

    void transfer(iterator position, iterator first, iterator last) {
        if (position != last) {

            (*( link_type( (*last.node).prev ) )).next = position.node;
            (*( link_type( (*first.node).prev ) )).next = last.node;
            (*( link_type( (*position.node).prev ) )).next = first.node;

            link_type tmp = link_type((*position.node).prev);

            (*position.node).prev = (*last.node).prev;
            (*last.node).prev = (*first.node).prev;
            (*first.node).prev = tmp;
        }
    }
};

    template <class T, class Alloc>
void list<T, Alloc>::clear()
{
    link_type cur = (link_type)node->next; //begin()
    while (cur != node) {
        link_type tmp = cur;
        cur = (link_type)cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

//移除值为value的元素
    template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value)
{
    iterator first = begin();
    iterator last = end();

    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

    template <class T, class Alloc>
void list<T, Alloc>::unique() 
{
    iterator first = begin();
    iterator last = end();

    if (first == last) return;

    iterator next = first;

    while (++next != last) {
        if (*first == *next) {
            erase(next);
        } else 
            first = next;
        next = first;
    }
}


    template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x)
{
    iterator first1 = begin();
    iterator last1 = end();

    iterator first2 = x.begin();
    iterator last2 = x.end();

    while (first1 != last1 && first2 != last2) {
        if (*first2 < *first1) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        } else 
            ++first1;
    }

    if (first2 != last2) 
        transfer(last1, first2, last2);
}

    template <class T, class Alloc>
void list<T, Alloc>::reverse() 
{
    if (node->next == node || link_type(node->next)->next == node) 
        return;
    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        transfer(begin(), old, first);
    }
}

    template <class T, class Alloc>
void list<T, Alloc>::sort() 
{
    if (node->next == node || link_type(node->next)->next == node)
        return;

    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;

    while (!empty()) {
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()) {
            counter[i].merge(carry);
            carry.swap(counter[i++]);
        }

        carry.swap(counter[i]);
        if (i == fill) ++fill;
    }

    for (int i = 1; i < fill; ++i) 
        counter[i].merge(counter[i-1]);

    swap(counter[fill-1]);
}

#endif
