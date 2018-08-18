#pragma once 
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "construct.h"
#include "./allocate.h"
#include "iterator.h"
#include "initialized.h"
#include <algorithm>

template <class T, class Alloc = alloc>
class vector {
public:
    //嵌套型别定义
    typedef T           value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

protected:
    typedef simple_alloc<value_type, Alloc> data_allocator;
    iterator start;         //目前使用的空间头部
    iterator finish;        //目前使用的空间尾
    iterator end_of_storage; //目前可用的空间尾
    
    void insert(iterator, size_type, const T&);
    void insert_aux(iterator position, const T& x);
    void deallocate() {
        if (start) 
            data_allocator::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin() const { return start; }
    iterator end() const { return finish; }
    size_type size() const { return size_type(end() - begin()); }
    size_type capacity() const
        { return size_type( end_of_storage - start ); }
    bool empty() const { return begin() == end(); }
    reference operator[](size_type n) { return *(begin() + n); }

    vector() : start(0), finish(0), end_of_storage(0) {}
    vector(size_type n, const T& value) { fill_initialize(n, value); }
    vector(int n, const T& value) { fill_initialize(n, value); }
    vector(long n, const T& value) { fill_initialize(n, value); }
    explicit vector(size_type n) { fill_initialize(n, T()); }

    ~vector() {
        destroy(start, finish);
        deallocate();
    }

    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    void push_back(const T& x) {
        if (finish != end_of_storage) {
            construct(finish, x);
            ++finish;
        } else {
            insert_aux(end(), x);
        }
    }

    void pop_back() {
        --finish;
        destroy(finish);
    }

    iterator erase(iterator position) {
        if (position + 1 != end()) 
            copy(position + 1, finish, position); //后续元素往前移动 将position+1 到finish的元素移动至position
        --finish;
        destroy(finish);
        return position;
    }

    iterator erase(iterator first, iterator last) {
        //把从last到finish的元素宝贝到first
        iterator i = copy(last, finish, first);
        destroy(i, finish);
        finish = finish - ( last - finish );
        return finish;
    }

    void resize(size_type new_size, const T& x) {
        if (new_size < size()) 
            erase(begin() + new_size, end());
        else 
            insert(end(), new_size - size(), x);
    }

    void resize(size_type new_size) { resize(new_size, T()); }
    void clear() { erase(begin(), end()); }
    
protected:
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }
};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator position, const T& x) 
{
    if (finish != end_of_storage) {
        //在备用空间起始处构造一个元素
        construct(finish, *(finish-1));
        //调整水位
        ++finish;
        T x_copy = x;
        //把从position到finish的元素后移到finish-1
        std::copy_backward(position, finish-2, finish-1);
        *position = x_copy;
    } else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;
        //原大小为0 配置1 
        //不为0就配置原大小两倍大小
        //前半段用来放置原数据
        iterator new_start = data_allocator::allocate(len);
        iterator new_finish = new_start;
        try {
            //将原vector拷贝至新vector
            new_finish = uninitialized_copy(start, position, new_start);
            //为新元素设定初值x
            construct(new_finish, x);
            ++new_finish;
            //将原vector的备用空间中的内容也忠实拷贝过来
            new_finish = uninitialized_copy(position, finish, new_finish);
        } catch (...) {
            destroy(new_start, new_finish);
            data_allocator::deallocate(new_start, len);
            throw;
        }

        //析构并释放原vector
        destroy(begin(), end());
        deallocate();

        //调整迭代器
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
    if (n != 0)  {
        if (size_type(end_of_storage - finish) >= n) {
            T x_copy = x;
            //计算插入点以后的现有元素个数
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            if (elems_after > n) {
                // "插入点之后的现有元素个数" 大于新增元素个数
                // 将末尾倒数n个元素复制到finish处
                uninitialized_copy(finish - n, finish, finish);
                //finish向后移动n个位置
                finish += n;
                //将插入点后剩余的原来元素右移到old_finish前面（注意！之前
                copy_backward(position, old_finish - n, old_finish);
                //将position后的元素插入 并将类型设置为x
                fill(position, position + n, x_copy);
            } else {
                //先将插入点后的元素补齐至n个元素 前半部分为原来的值  后半部分 即n - elems_after为新值
                uninitialized_fill_n(finish, n - elems_after, x_copy);
                //finish向后移动
                finish += n - elems_after;
                //将插入点到原来的finish即old_finish移动至新的finish以后
                uninitialized_fill_n(position, old_finish, finish);
                //finish 向后移动 此时finish共向后移动了n个位置
                finish += elems_after;
                //将新元素fill到position到old_finish 的位置
                fill(position, old_finish, x_copy);
            }
        } else {
               //备用空间不足
            const size_type old_size = size();
            const size_type len = old_size + (old_size > n ? old_size : n);
            //配置新的空间
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                //将旧vector的插入点之前的元素复制到新空间 
                new_finish = uninitialized_copy(start, position, new_start);
                //将新增元素复制到新空间
                new_finish = uninitialized_fill_n(new_finish, n, x);
                //原vector插入点之后的元素复制过来
                new_finish = uninitialized_fill_n(position, finish, new_finish);
            } 
            catch(...) {
                //有异常就实现 commit or rollback
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            destroy(start, finish);
            deallocate();

            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

#endif
