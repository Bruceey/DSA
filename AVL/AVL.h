//
// Created by 17634 on 2022/8/29.
//

#ifndef DSA_AVL_H
#define DSA_AVL_H

#include "../BST/BST.h"

/******************************************************************************************
 * 在左、右孩子中取更高者
 * 在AVL平衡调整前，借此确定重构方案
 ******************************************************************************************/
#define tallerChild(x) ( \
   stature( (x)->lc ) > stature( (x)->rc ) ? (x)->lc : ( /*左高*/ \
   stature( (x)->lc ) < stature( (x)->rc ) ? (x)->rc : ( /*右高*/ \
   IsLChild( * (x) ) ? (x)->lc : (x)->rc /*等高：与父亲x同侧者（zIg-zIg或zAg-zAg）优先*/ \
   ) \
   ) \
)

#define Balanced(x) ( stature((x).lc) == stature((x).rc) ) //理想平衡条件
#define BalFac(x) ( stature( (x).lc ) - stature( (x).rc ) ) //平衡因子
#define AvlBalanced(x) ( ( -2 < BalFac(x) ) && ( BalFac(x) < 2 ) ) //AVL平衡条件

template<typename T>
class AVL : public BST<T> { //由BST派生AVL树模板类
public:
    BinNodePosi<T> insert(const T& e); //插入（重写）
    bool remove(const T& e); //删除（重写）
    // BST::search()等其余接口可直接沿用
};

template<typename T>
BinNodePosi<T> AVL<T>::insert(const T &e) { //将关键码e插入AVL树中
    BinNodePosi<T> & x = this->search(e); if (x) return x; //确认目标不存在
    BinNodePosi<T> xx = x = new BinNode<T>(e, this->_hot); this->_size++; //创建新节点x
    //此时，x的父亲及祖先都可能失衡
    for (BinNodePosi<T> g = this->_hot; g; g = g->parent) //从x之父出发向上，逐层检查各代祖先g
        if (!AvlBalanced(*g)) { //一旦发现g失衡，则（采用“3 + 4”算法）使之复衡，并将子树
            FromParentTo(*g) = this->rotateAt(tallerChild(tallerChild(g))); //重新接入原树
            break; //局部子树复衡后，高度必然复原；其祖先亦必如此，故调整结束
        } else //否则（g仍平衡）
            this->updateHeight(g); //只需更新其高度（注意：即便g未失衡，高度亦可能增加）
    return xx; //返回新节点位置
} //无论e是否存在于原树中，总有AVL::insert(e)->data == e

template<typename T>
bool AVL<T>::remove(const T &e) { //从AVL树中删除关键码e
    BinNodePosi<T> & x = this->search(e); if (!x) return false; //确认目标存在
    removeAt(x, this->_hot); this->_size--; ////先按BST规则删除之（此后，原节点之父_hot及其祖先均可能失衡）
    for (BinNodePosi<T> g = this->_hot; g; g = g->parent) { //从_hot出发向上，逐层检查各代祖先g
        if (!AvlBalanced(*g)) ////一旦发现g失衡，则（采用“3 + 4”算法）使之复衡，并将该子树联至
            g = FromParentTo(*g) = this->rotateAt(tallerChild(tallerChild(g))); //原父亲
        this->updateHeight(g); //更新高度（注意：即便g未失衡或已恢复平衡，高度均可能降低）
    } //可能需做Omega(logn)次调整——无论是否做过调整，全树高度均可能降低
    return true; //删除成功
}

#endif //DSA_AVL_H
