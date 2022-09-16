//
// Created by 17634 on 2022/9/2.
//

#ifndef DSA_QUADLIST_H
#define DSA_QUADLIST_H

#include "QNode.h"

template<typename T>
struct QuadList { //四联列表
    int _size; //规模
    QNodePosi<T> header, trailer; //头哨兵、尾哨兵
    void init(); //初始化
    int clear(); //清除所有节点
    QuadList() { init(); } //构造
    ~QuadList() { clear(); delete header; delete trailer; } //析构
    T remove(QNodePosi<T> p); //删除（合法）位置p处的节点，返回被删除节点的数值
    QNodePosi<T> insert(T const& e, QNodePosi<T> p, QNodePosi<T> b = nullptr); ////将e作为p的后继、b的上邻插入
    void traverse( void (* ) ( T& ) ); //遍历各节点，依次实施指定操作（函数指针，只读或局部修改）
    template <typename VST> void traverse ( VST& ); //遍历
};

template<typename T>
void QuadList<T>::init() { //Quadlist初始化，创建Quadlist对象时统一调用
    header = new QNode<T>; //头哨兵
    trailer = new QNode<T>; //尾哨兵
    header->succ = trailer; header->pred = nullptr;
    trailer->pred = header; trailer->succ = nullptr;
    header->above = trailer->above = nullptr; //纵向的后继置空
    header->below = trailer->below = nullptr; //纵向的前驱置空
    _size = 0;
} //如此构造的四联表，不含任何实质的节点，且暂时与其它四联表相互独立

template<typename T> //删除Quadlist内位置p处的节点，返回其中存放的词条
T QuadList<T>::remove(QNodePosi<T> p) { //assert: p为Quadlist中的合法位置
    p->pred->succ = p->succ;
    p->succ->pred = p->pred;
    _size--;
    T e = p->entry;
    delete p;
    return e;
}

template<typename T>
int QuadList<T>::clear() { //清空Quadlist
    int oldSize = _size;
    while (0 < _size) remove(header->succ);
    return oldSize;
}

template<typename T> //将e作为p的后继、b的上邻插入Quadlist
QNodePosi<T> QuadList<T>::insert(const T &e, QNodePosi<T> p, QNodePosi<T> b) {
    _size++;
    return p->insert(e, b); //返回新节点位置（below = NULL）
}

template<typename T> //遍历Quadlist，对各节点依次实施visit操作
void QuadList<T>::traverse(void (*visit)(T &)) { //利用函数指针机制，只读或局部性修改
    QNodePosi<T> p = header;
    while ((p = p->succ) != trailer) visit(p->entry);
}

template<typename T>
template<typename VST>
void QuadList<T>::traverse(VST &visit) { //利用函数对象机制，可全局性修改
    QNodePosi<T> p = header;
    while ( ( p = p->succ ) != trailer ) visit ( p->entry );
}

#endif //DSA_QUADLIST_H
