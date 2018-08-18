#include <iostream>
#include "vector.h"
#include "iterator.h"
#include "list.h"
int main()
{
    std::cout << "success!" << std::endl;
    vector<int> vec;
    
    vec.push_back(1);
    vec.push_back(2);
    std::cout << vec.size() << std::endl;

    std::cout << vec[0] << std::endl;
    vector<int>::iterator iter = vec.begin();
    std::cout << *iter << std::endl;
    iter++;
    std::cout << *iter << std::endl;
    vec.pop_back();
    std::cout << *vec.begin() << std::endl;

    std::cout << "***********list******************" << std::endl;

    list<int> lst;
    lst.push_back(45);
    list<int>::iterator begin = lst.begin();
    std::cout << *begin << std::endl;
    lst.push_back(233);
    std::cout << *begin << std::endl;
    lst.push_front(23);
    list<int>::iterator beg = lst.begin();
    std::cout << *beg << std::endl;
    
    lst.push_back(34);
    lst.push_back(44);
    lst.push_back(44);
    lst.push_back(44);
    beg = lst.begin();
    list<int>::iterator end = lst.end();

    std::cout << "*********through all****" << std::endl;
    while (beg != end) {
        std::cout << *beg << std::endl;
        beg++;
    }

    std::cout << "*********after unique****" << std::endl;

    lst.unique();
    beg = lst.begin();
    end = lst.end();
    while (beg != end) {
        std::cout << *beg << std::endl;
        beg++;
    }
    
    std::cout << "***********after sort **********" << std::endl;
    lst.sort();
    beg = lst.begin();
    end = lst.end();
    while (beg != end) {
        std::cout << *beg << std::endl;
        beg++;
    }
    


}
