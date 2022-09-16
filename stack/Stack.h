//
// Created by 17634 on 2022/8/25.
//

#ifndef DSA_STACK_H
#define DSA_STACK_H

#include "../vector/Vector.h"
//#include "../list/List.h"

//利用Vector模板类定义Stack
template<typename T>
class Stack: public Vector<T> { //将向量的首/末端作为栈底/顶
public: //原有接口一概沿用
    void push(T const& e) { this->insert(e); } //入栈：等效于将新元素作为向量的末元素插入
    T pop() { return this->remove(this->size() - 1); } //出栈：等效于删除向量的末元素
    T& top() { return (*this)[this->size() - 1]; } //取顶：直接返回向量的末元素
};


////利用List模板类定义Stack
//template<typename T>
//class Stack: public List<T> { //将列表的首/末端作为栈底/顶
//public:
//    void push(T const& e) { this->insertAsLast(e); } //入栈：等效于将新元素作为列表的末元素插入
//    T pop() { return this->remove(this->last()); } //出栈：等效于删除列表的末元素
//    T& top() { return this->last()->data; } //取顶：直接返回列表的末元素
//};




#endif //DSA_STACK_H
