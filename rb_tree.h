#pragma once
#ifndef _RB_TREE_
#define _RB_TREE_

#include "exception.h"
#include "allocate.h"
#include "type_traits.h"
#include "initialized.h"
#include "iterator.h"

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false; //红色
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base
{
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)
    {
        while (x->left != 0)
            x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while (x->right != 0) 
            x = x->right;
        return x;
    }
};

template <class Value>
struct __rb_tree_node : public __rb_tree_node_base
{
    typedef __rb_tree_node<Value>* link_type;
    Value value_field;
};

struct __rb_tree_base_iterator
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;
    
    base_ptr node; //指向红黑树对应节点 与红黑树产生结合

    void increment()
    {
        if (node->right != 0)
        {
            node = node->right;
            while (node->left != 0)
                node = node->left;
        }
        else 
        {
            base_ptr y = node->parent;
            while (node == y->right)
            {
                node = y;
                y = y->parent;
            }
            if (node->right != y)
                node = y;
        }
    }

    void decrement()
    {
        //此时node指向root 还有一个header与root互相指
        if (node->color == __rb_tree_red && 
            node->parent->parent == node)
            node = node->right;
        else if (node->left != 0)  //如果左边不为0 到左子树的最右节点
        {
            base_ptr y = node->left;
            while (y->right != 0)
                y = y->right;
            node = y;
        }
        else 
        {  
            //如果没有左子树 用一个指针指向node的parent
            base_ptr y = node->parent;
            while (node == y->left)
            {
                node = y;
                y = y->parent;
            }
            //一直到node为y的右节点 此时y即为上一个节点
            node = y;
        }
    }
};

template <class Value, class Ref, class Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
    typedef __rb_tree_iterator<Value, const Value&, const Value*>   const_iterator;
    typedef __rb_tree_iterator<Value, Value&, Value*>               iterator;
    typedef __rb_tree_iterator<Value, Ref, Ptr>                     self;
    typedef __rb_tree_node<Value>*                                  link_type;
    typedef Value   value_type;
    typedef Ref     reference;
    typedef Ptr     pointer;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { node = x; }
    __rb_tree_iterator(const iterator& it) { node = it.node; }

    reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
    pointer operator->() const { return &(operator*()); }
#endif

    self& operator++() { increment(); return *this; }
    self operator++(int)
    {
        self temp = *this;
        ++*this;
        return temp;
    }

    self& operator--() { decrement(); return *this; }
    self operator--(int)
    {
        self temp = *this;
        --*this;
        return temp;
    }

};

template <class Key, class Value, class KeyOfValue, 
         class Compare, class Alloc>
class rb_tree
{
protected:
    typedef void*   void_pointer;
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> rb_tree_node;
    typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
    typedef __rb_tree_color_type color_type;

public:
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef rb_tree_node*       link_type;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

protected:
    link_type get_node() { return rb_tree_node_allocator::allocate(); }
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

    link_type create_node(const value_type& x) 
    {
        link_type tmp = get_node();
        __STL_TRY 
        {
            construct(&tmp->value_field, x);
        }
        __STL_UNWIND(put_node(tmp));
        return tmp;
    }

    link_type clone_node(link_type x)
    {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = x->left;
        tmp->right = x->right;
        tmp->parent = x->parent;
        return tmp;
    }

    void destroy_node(link_type node)
    {
        destroy(&node->value_field);
        put_node(node);
    }

protected:
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type& root() const { return (link_type&)header->parent; }
    link_type& leftmost() const { return (link_type&)header->left; }
    link_type& rightmost() const { return (link_type&)header->right; }

    //以下六个函数用来方便取得x的成员
    static link_type& left(link_type x)
    { return (link_type&) (x->left); }
    static link_type& right(link_type x)
    { return (link_type&) (x->right); }
    static link_type& parent(link_type x)
    { return (link_type&) (x->parent); }
    static reference value(link_type x)
    { return x->value_field; }
    static const Key& key(link_type x)
    { return KeyOfValue() (value(x)); }
    static color_type& color(link_type x)
    { return (color_type&) (x->color); }

    static link_type& left(base_ptr x)
    { return (link_type&) (x->left); }
    static link_type& right(base_ptr x)
    { return (link_type&) (x->right); }
    static link_type& parent(base_ptr x)
    { return (link_type&) (x->parent); }
    static reference value(base_ptr x)
    { return ((link_type)x)->value_field; }
    static const Key& key(base_ptr x)
    { return KeyOfValue() (value(x)); }
    static color_type& color(base_ptr x)
    { return (color_type&) (x->color); }
    
    //极大值和极小值
    static link_type minimum(link_type x)
    { return (link_type) __rb_tree_node_base::minimum(x); }
    static link_type maximum(link_type x)
    { return (link_type) __rb_tree_node_base::maximum(x); }

public:
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

private:
    iterator __insert(base_ptr x, base_ptr y, const value_type& v);
    link_type __copy(link_type x, link_type y);
    void erase(link_type x);
    void erase(iterator pos);
    void init()
    {
        header = get_node();
        color(header) = __rb_tree_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }

public:
    rb_tree(const Compare& comp = Compare()) :
        node_count(0), key_compare(comp) { init(); }

    ~rb_tree()
    {
        clear();
        put_node(header);
    }

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
        operator= (const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

public:
    Compare key_com() const { return key_compare; }
    iterator begin() { return leftmost(); }
    iterator end() { return header; }
    bool empty() const { return node_count == 0; } 
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }
    void clear();
public:
    std::pair<iterator, bool> insert_unique(const value_type& x);
    void insert_unique(const_iterator first, const_iterator last);
    iterator insert_equal(const value_type& x);
    iterator find(const Key& k);
};

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v)
{
   link_type y = header;
   link_type x = root();
   while (x != 0)
   {
       y = x;
       //KeyOfValue 将value转化成key的函数 
       x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
   }

   return __insert(x, y, v);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v)
{
    link_type y = header;
    link_type x = root();
    bool comp = true;
    /**
     * 首先 从上往下 大于等于当前节点 往右
     * 小于当前节点 往左 
     * 一直到叶子节点
     * **/
    while (x != 0)
    {
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x)); //将value的key取出来与当前点比较
        x = comp ? left(x) : right(x);
    }

    /**
     * 此时 y为叶子节点
     * 如果comp为1 说明上面的while结束时 需要往左边插节点 
     * 此时 如果正好是最左边的节点 直接插入就可以
     * **/
    iterator j = iterator(y);
    if (comp)
    {
        if (j == begin()) 
            return std::pair<iterator, bool>(__insert(x, y, v), true);
        /***
         * 但是 如果不是的话 我们将迭代器减一 此时刚好指向 当前节点前一个节点
         * 即为插入位置 
         * **/
        else 
            --j;
    }
    //如果不同 就插入
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return std::pair<iterator, bool>(__insert(x, y, v), true);

    //否则说明有重复节点 返回false 说明插入失败
    return std::pair<iterator, bool>(j, false);
}

template <class Key, class Value, class KeyOfValue, class Comp, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Comp, Alloc>::
                insert_unique(const_iterator first, const_iterator last)
{
    for ( ; first != last; first++ )
        insert_unique(*first);
}
/***
 * x 要插入的节点
 * y 要插入的父节点
 * v 要插入的值 
 * **/
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
__insert(base_ptr x_, base_ptr y_, const value_type& v)
{
    link_type x = (link_type)x_;
    link_type y = (link_type)y_;
    link_type z;

    if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y)))
    {
        z = create_node(v);
        left(y) = z;
        if (y == header)
        {
            root() = z;
            rightmost() = z;
        } 
        else if (y == leftmost())
        {
            leftmost() = z;
        }
    }
    else 
    {
        z = create_node(v);
        right(y) = z;
        if (y == rightmost())
            rightmost() = z;
    }

    parent(z) = y;
    left(z) = 0;
    right(z) = 0;

    __rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}

//x为旋转点
inline void 
__rb_tree_rotate_left(__rb_tree_node_base* x,
                      __rb_tree_node_base*& root)
{
    __rb_tree_node_base* y = x->right;
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->parent = x->parent;

    //将x对其父节点的关系接收过来
    if (x == root)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

inline void __rb_tree_rotate_right(__rb_tree_node_base* x,
                                   __rb_tree_node_base*& root)
{
    __rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;

    if (x == root)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else 
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

//x为新增节点
inline void 
__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base* root)
{
    x->color = __rb_tree_red;
    //x的parent为红色
    while (x != root && x->parent->color == __rb_tree_red)
    {
        if (x->parent == x->parent->parent->left)
        {
            __rb_tree_node_base* y = x->parent->parent->right;
            //自上而下 如果有某节点两个子节点都是红色 
            //就将子节点变为黑色 并将该节点变为红色
            if (y && y->color == __rb_tree_red)
            {
                x->parent->color = __rb_tree_black;
                y->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                    x = x->parent->parent;
            }
            else //无伯父节点 或者 为黑 
            {
                if (x == x->parent->right)              
                { 
                    //新节点为父节点的右结点 需要双旋转
                    //旋转点为插入点的父节点 先左旋 再右旋
                    x = x->parent;
                    __rb_tree_rotate_left(x, root);
                }

                //左旋后改变颜色
                //新增节点一定为红色　其祖父节点为黑色　将颜色改变　
                //旋转后　原父节点为新节点的左孩子　此时ｘ指向新节点的左孩子（原父节点　
                //我们应当保证每条路径上的黑色总数不变
                //所以　右旋时　将Ｇ改成红色　将此时x->parent　（新的ｘ　改为黑色
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        else  //父节点为祖父节点的右节点 
        {
            __rb_tree_node_base* y = x->parent->parent->left;
            if (y && y->color == __rb_tree_red)
            {
                //top down prodcure
                y->color = __rb_tree_black;
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
            }
            else 
            {
                if (x == x->parent->left)
                {
                    x = x->parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->parent->color = __rb_tree_black;
                x->parent->parent->color = __rb_tree_red;
                __rb_tree_rotate_left(x->parent->parent, root);

            }
        }
        root->color = __rb_tree_black;
    }
}

inline __rb_tree_node_base*
__rb_tree_rebalance_for_erase(__rb_tree_node_base* z, 
                              __rb_tree_node_base* root, 
                              __rb_tree_node_base* leftmost, 
                              __rb_tree_node_base* rightmost)
{
    __rb_tree_node_base* y = z;         //调整后实际被删除的指针
    __rb_tree_node_base* x = 0;         //ｙ的某个子节点
    __rb_tree_node_base* x_parent = 0;  //ｘ的父节点

    if (y->left == 0)   //ｚ最多有一个非空子节点　y == z
        x = y->right;   //ｘ此时可能为空
    else 
        if (y->right == 0)
            x = y->left;        //此时　ｘ一定不不为空　ｚ一定只有一个非空节点　即ｘ
        else {              //此时　ｚ两个节点都非空
            y = z->right;
            while (y->left != 0)
            {
                y = y->left;
                x = y->right;
            }
        }
    /***
     * 此时　ｙ指向了ｚ的下一个节点　（为什么不直接y++呢？？
     * x是y的右孩子 ***/
    if (y != z) //这部分做的是　将ｙ与ｚ的值交换　交换了位置　但是颜色没有交换
    {
        //将ｚ的左孩收过来
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right)
        {
            x_parent = y->parent;
            if (x)
                x->parent = y->parent;
            y->parent->left = x; //x顶上y的位置
            y->right = z->right;
            z->right->parent = y; //将y的右孩子也接收过来
        }
        else   //如果 y是z的右孩子 即一开始的时候y的左孩子是0
                //那么直接让x指向y  
            x_parent = y;    
        //接管z的上下文
        if (root == z)
            root = y;
        else if (z->parent->left == z)
            z->parent->left = y;
        else 
            z->parent->right = y;
        y->parent = z->parent;
        //将颜色换回来 即颜色未改变 只有值改变了
        std::swap(y->color, z->color);
        y = z; //此时 y指向应该被删除的位置 即原z的下一个节点
    }
    else
    {
        //此时 x应该要去替代z的位置
        x_parent = y->parent;
        if (x)
            x->parent = y->parent;
        //x取代z的上下文
        if (root == z)
            root = x;  
        else 
            if (z->parent->left == z)
                z->parent->left = x;
            else 
                z->parent->right = x;

        //如果z是极大或极小值
        //那么也要相应的更新
        if (leftmost == z)
        {
            if (z->right == 0) //此时 right为0 x对应的应该是z的right
                leftmost = z->parent;
            else   //否则 x应该指向z的left
                leftmost = __rb_tree_node_base::minimum(x);
        }
        if (rightmost == z)
        {
            if (z->left == 0) //此时 left为0 x指向z的left
                rightmost = z->parent;
            else        //此时x指向z的right
                rightmost = __rb_tree_node_base::maximum(x);
        }
    }

    //y不为红色 不可以直接删除 让x顶上
    if (y->color != __rb_tree_red)
    {
        //此时 x的分支相对于兄弟w少了一个黑色节点
        while (x != root && (x == 0 || x->color == __rb_tree_black))
            if (x == x_parent->left)
            {
                //找到x的兄弟节点
                __rb_tree_node_base* w = x_parent->right;
                if (w->color == __rb_tree_red) //如果为红色
                {
                    w->color = __rb_tree_black;//将其变成黑色 
                    x_parent->color = __rb_tree_red; //父节点变为红色 
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }

                //如果兄弟w的两个儿子都为黑色
                if ((w->left == 0 || 
                     w->left->color == __rb_tree_black) && 
                    (w->right == 0 || 
                     w->right->color == __rb_tree_black))
                {
                    //将w的颜色改成红色 
                    //并上溯
                    w->color = __rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } 
                else 
                {
                    //如果w的右孩子为黑色 左孩子的红色
                    if (w->right == 0 || 
                        w->right->color == __rb_tree_black)
                    {
                        w->color = __rb_tree_red;
                        //将左孩子改成黑色 并右旋
                        if (w->left != 0)
                            w->left->color = __rb_tree_black;
                        __rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    //此时一定是左孩子为黑色 
                    //右孩子为红色 
                    //此时将右孩子改为黑色
                    //以x_parent为节点左旋
                    //结束以后 
                    //原w的右孩子 新增一个黑色节点
                    //原w的左孩子被转移到x_parent的右孩子下 
                    //而x_parent成为 原x 与 原w 的左孩子的新父母 
                    //为x以及原w左孩子新增一个黑色节点 即它本身
                    w->color = x_parent->color;
                    x->parent->color = __rb_tree_black;
                    if (w->right)
                        w->right->color = __rb_tree_black;
                    __rb_tree_rotate_left(x_parent, root);
                    break;
                }
            }
            else //x是parent的右节点 同上面相同
            {
                __rb_tree_node_base* w = x_parent->left;
                if (w->color == __rb_tree_red)
                {
                    w->color = __rb_tree_black;
                    x_parent->color = __rb_tree_red;
                    __rb_tree_rotate_left(x_parent, root);
                    w = x_parent->left;
                }

                if ((w->left == 0 || 
                     w->left->color == __rb_tree_black) && 
                    (w->right == 0 ||
                     w->right->color == __rb_tree_black))
                {
                    w->color = __rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } 
                else 
                {
                    if (w->left == 0 || 
                        w->left->color == __rb_tree_black)
                    {
                        w->color = __rb_tree_red;
                        if (w->right != 0)
                            w->right->color = __rb_tree_black;
                        __rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }

                    w->color = x_parent->color;
                    x_parent->color = __rb_tree_black;
                    if (w->left)
                        w->left->color = __rb_tree_black;
                    __rb_tree_rotate_right(x_parent, root);
                    break;
                }

            }
        if (x)
            x->color = __rb_tree_black;
    }

    return y;
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(link_type x)
{
    while (x != 0)
    {
        erase(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;
    }
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(
        typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator pos)
{
    link_type y = 
        (link_type) __rb_tree_rebalance_for_erase(pos.node, 
                                                 header->parent,
                                                 header->left, 
                                                 header->right);
    destroy_node(y);
    --node_count;
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k)
{
    link_type y = header;
    link_type x = root();

    while (x != 0)
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else 
            x = right(x);
    iterator j = iterator(y);
    return (j == end() || key_compare(k, key(j.node))) ? end() : j; 
}

#endif

