//
// Created by 17634 on 2022/9/2.
//

#ifndef DSA_QNODE_H
#define DSA_QNODE_H

#include "../Entry/Entry.h"

template<typename T> struct QNode;
template<typename T> using QNodePosi = QNode<T>*; //跳转表节点位置

template<typename T>
struct QNode { //四联节点
    T entry; //所存词条
    QNodePosi<T> pred, succ, above, below; //前驱、后继、上邻、下邻
    QNode(T e = T(), QNodePosi<T> p = nullptr, QNodePosi<T> s = nullptr,
          QNodePosi<T> a = nullptr, QNodePosi<T> b = nullptr)
          : entry(e), pred(p), succ(s), above(a), below(b) {}
    QNodePosi<T> insert(T const& e, QNodePosi<T> b = nullptr); //将e作为当前节点的后继、b的上邻插入
};

template<typename T> //将e作为当前节点的后继、b的上邻插入Quadlist
QNodePosi<T> QNode<T>::insert(const T &e, QNodePosi<T> b) {
    QNodePosi<T> x = new QNode<T> (e, this, succ, nullptr, b);
    succ->pred = x; succ = x; //水平逆向链接
    if (b) b->above = x; //设置垂直逆向链接
    return x; //返回新节点的位置
}

#endif //DSA_QNODE_H
