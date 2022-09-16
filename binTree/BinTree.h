//
// Created by 王锐 on 2021/11/6.
//

#ifndef DSA_BINTREE_H
#define DSA_BINTREE_H

#include "BinNode.h"

template<typename T>
class BinTree { //二叉树模板类
protected:
    int _size; //规模
    BinNodePosi<T> _root; //根节点
    virtual int updateHeight(BinNodePosi<T> x); //更新节点x的高度
    void updateHeightAbove(BinNodePosi<T> x); //更新节点x及祖先的高度
public:
    BinTree() : _size(0), _root(nullptr) {} //构造函数
    ~BinTree() {if ( 0 < _size ) remove ( _root );} //析构函数

    int size() const {return _size;}  //规模
    bool empty() const {return !_root; } //判空
    BinNodePosi<T> root() const {return _root;}  //树根
//插入和删除
    BinNodePosi<T> insert ( T const & ); //插入根节点
    BinNodePosi<T> insert ( T const &, BinNodePosi<T> ); //插入左孩子
    BinNodePosi<T> insert ( BinNodePosi<T>, T const & ); //插入右孩子
    BinNodePosi<T> attach ( BinTree<T> * &, BinNodePosi<T> ); //接入左子树
    BinNodePosi<T> attach ( BinNodePosi<T>, BinTree<T> * & ); //接入右子树
    int remove ( BinNodePosi<T> ); //子树删除
    BinTree<T>* secede ( BinNodePosi<T> ); //子树分离
//遍历
    template <typename VST> //操作器
    void travLevel ( VST& visit ) { if ( _root ) _root->travLevel ( visit ); } //层次遍历
    template <typename VST> //操作器
    void travPre ( VST& visit ) { if ( _root ) _root->travPre ( visit ); } //先序遍历
    template <typename VST> //操作器
    void travIn ( VST& visit ) { if ( _root ) _root->travIn ( visit ); } //中序遍历
    template <typename VST> //操作器
    void travPost ( VST& visit ) { if ( _root ) _root->travPost ( visit ); } //后序遍历

    bool operator== ( BinTree<T> const& t ) //判等器
    { return _root && t._root && ( _root == t._root ); }

}; //BinTree



//BinTree具体实现
template<typename T>
int BinTree<T>::updateHeight(BinNodePosi<T> x) { //更新节点x的高度
    return x->height = 1 + max(stature(x->lc), stature(x->rc));
}

template<typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi<T> x) { //更新节点x及祖先的高度
    while (x) {
        updateHeight(x);
        x = x->parent;
    }
}

template<typename T>
BinNodePosi<T> BinTree<T>::insert(const T & e) {  //将e当作根节点插入空的二叉树
    _size = 1;
    return _root = new BinNode<T>(e);
}

template<typename T>
BinNodePosi<T> BinTree<T>::insert(const T &e, BinNodePosi<T>x) { //e插入为x的左孩子
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lc;
}

template<typename T>
BinNodePosi<T> BinTree<T>::insert(BinNodePosi<T>x, const T &e) { //e插入为x的右孩子
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rc;
}

template<typename T> //将S当作节点x的左子树接入二叉树，S本身置空
BinNodePosi<T> BinTree<T>::attach(BinTree<T>* &S, BinNodePosi<T> x) { // assert: x->lc == nullptr
    if (x->lc = S->_root) x->lc->parent = x; //接入
    _size += S->_size;
    updateHeightAbove(x); //更新x及其祖先高度
    S->_root = nullptr;
    S->_size = 0;  // 此处直接将_size置0，故
    release(S); // release时进入~BinTree()，函数体要求_size>0才执行. 所以不会释放S树原来的所有节点
    S = nullptr;
    return x; //释放原树，返回接入位置
}

template<typename T> //将S当作节点x的右子树接入二叉树，S本身置空
BinNodePosi<T> BinTree<T>::attach(BinNodePosi<T> x, BinTree<T> *&S) { // assert: x->rc == nullptr
    if (x->rc = S->_root) x->rc->parent = x; //接入
    _size += S->_size;
    updateHeightAbove(x); //更新x及其祖先高度
    S->_root = nullptr;
    S->_size = 0; // 此处直接将_size置0，故
    release(S); // release时进入~BinTree()，函数体要求_size>0才执行. 所以不会释放S树原来的所有节点
    S = nullptr;
    return x; //释放原树，返回接入位置
}

template<typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的个数
static int removeAt(BinNodePosi<T> x) { //assert: x为二叉树中的合法位置
    if (!x) return 0; //递归基：空树
    int n = 1 + removeAt(x->lc) + removeAt(x->rc); //递归释放左、右子树
    release(x->data);
    release(x);
    return n; //释放被摘除节点，并返回删除节点总数
}

template<typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的个数
int BinTree<T>::remove(BinNodePosi<T> x) { //assert: x为二叉树中的合法位置
    FromParentTo(*x) = nullptr; //切断来自父节点的指针
    updateHeightAbove(x->parent); //更新祖先高度
    int n = removeAt(x);
    _size -= n;
    return n; //删除子树x，更新规模，返回删除节点总数
}

template<typename T> //二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
BinTree<T> *BinTree<T>::secede(BinNodePosi<T> x) { //assert: x为二叉树中的合法位置
    FromParentTo(*x) == nullptr; //切断来自父节点的指针
    updateHeightAbove(x->parent); //更新原树中所有祖先的高度
    BinTree<T>* S = new BinTree<T>;
    S->_root = x;
    x->parent = nullptr; //新树以x为根
    S->_size = x->size();
    _size -= S->_size;
    return S; //更新规模，返回分离出来的子树
}

#endif //DSA_BINTREE_H
