//
// Created by 王锐 on 2021/11/3.
//

#ifndef DSA_LISTNODE_H
#define DSA_LISTNODE_H

using Rank = int;
template <typename T> struct ListNode;
template <typename T> using ListNodePosi = ListNode<T>*; //列表节点位置

template<typename T>
struct ListNode{    //节点模版类
    //成员
    T data; ListNodePosi<T> pred; ListNodePosi<T> succ; //数值、前驱、后继
    //构造函数
    ListNode() = default;
    ListNode(T e, ListNodePosi<T> p= nullptr, ListNodePosi<T> s = nullptr)
    : data(e), pred(p), succ(s){}
    // 操作接口
    ListNodePosi<T> insertAsPred(T const& e){ // 插入到节点前
        ListNodePosi<T> x = new ListNode(e, pred, this); //创建新节点
        pred->succ = x; pred = x;   // 不能颠倒
        return x;
    }
    ListNodePosi<T> insertAsSucc(T const& e){ //插入到节点后
        ListNodePosi<T> x = new ListNode(e, this, succ);
        succ->pred = x; succ = x;   // 不能颠倒
        return x;
    }
};

#endif //DSA_LISTNODE_H
