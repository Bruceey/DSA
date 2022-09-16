//
// Created by 17634 on 2022/8/25.
//

#ifndef DSA_QUEUE_H
#define DSA_QUEUE_H

#include "../list/List.h"

//以List为基类派生出的
template<typename T>
class Queue: public List<T> { //队列模板类
public:
    void enqueue(T const& e) { this->insertAsLast(e); } //入队：尾部插入
    T dequeue() { return this->remove(this->first()); } //出队：首部删除
    T& front() { return this->first()->data; } //队首
};

#endif //DSA_QUEUE_H
