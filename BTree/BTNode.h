//
// Created by 17634 on 2022/8/31.
//

#ifndef DSA_BTNODE_H
#define DSA_BTNODE_H

#include "../vector/Vector.h"
template<typename T> struct BTNode;
template<typename T> using BTNodePosi = BTNode<T>*; //B-树节点位置

template<typename T>
struct BTNode { //B-树节点模板类
// 成员（为简化描述起见统一开放，读者可根据需要进一步封装）
    BTNodePosi<T> parent; //父节点
    Vector<T> key; //关键码向量
    Vector<BTNodePosi<T>> child; //孩子向量（总比关键码多一个）
// 构造函数
    BTNode() {parent = nullptr; child.insert(nullptr);}
    BTNode ( T e, BTNodePosi<T> lc = nullptr, BTNodePosi<T> rc = nullptr ) {
        parent = nullptr; key.insert ( e ); //作为根节点只有一个关键码，以及
        child.insert ( lc ); if ( lc ) lc->parent = this; //左孩子
        child.insert ( rc ); if ( rc ) rc->parent = this; //右孩子
    }
};

#endif //DSA_BTNODE_H
