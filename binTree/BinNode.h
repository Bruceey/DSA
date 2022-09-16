//
// Created by 王锐 on 2021/11/6.
//

#ifndef DSA_BINNODE_H
#define DSA_BINNODE_H

#include "BinNode_macro_BASIC.h"
#include "../_share/util.h"
#include "../_share/release.h"
#include "../queue/Queue.h"
#include "../stack/Stack.h"

#if defined( DSA_REDBLACK_H )
#define stature(p) ((p) ? (p)->height : 0) //红黑树节点的黑高度（NULL视作外部节点，对应于0）
#else
#define stature(p) ((p) ? (p)->height : -1) //其余BST中节点的高度（NUll视作空树，对应于-1）
#endif

typedef enum {RB_RED, RB_BLACK} RBColor; //节点颜色
template<typename T> struct BinNode;
template<typename T> using BinNodePosi = BinNode<T>*; //节点位置

template<typename T>
struct BinNode { //二叉树节点模板类
    T data; //数据
    BinNodePosi<T> parent, lc, rc;  //父亲、左孩子、右孩子
    int height; //以其为根的子树高度
    int npl; //Null Path Length(左式堆）
    RBColor color;  // 颜色（红黑树）

//构造函数
    BinNode(): parent(nullptr), lc(nullptr), rc(nullptr), height(0), npl(1), color(RB_RED) {}
    BinNode(T e, BinNodePosi<T> p= nullptr, BinNodePosi<T> lc= nullptr, BinNodePosi<T> rc= nullptr, int h=0, int l=1, RBColor c=RB_RED):
    data(e), parent(p), lc(lc), rc(rc), height(h), npl(l), color(c) {}

//操作接口
    int size(); //以其为根的子树规模
    BinNodePosi<T> insertAsLC(T const& e);  //作为左孩子插入新节点
    BinNodePosi<T> insertAsRC(T const& e);  //作为右孩子插入新节点

//遍历
    BinNodePosi<T> succ();  //(中序遍历意义下)当前节点的直接后继
    template<typename VST> void travLevel( VST& visit ); //子树层次遍历
    template<typename VST> void travPre( VST& visit ); //子树前序遍历
    template<typename VST> void travIn( VST& visit ); //子树中序遍历
    template<typename VST> void travPost( VST& visit ); //子树后序遍历

//比较器、判等器
    bool operator< (BinNode const& bn) {return data < bn.data;} //小于
    bool operator== (BinNode const& bn) {return data == bn.data;} //等于

}; //BinNode



//BinNode结构体的实现
template<typename T>
int BinNode<T>::size() {
    int s = 1; //计入本身
    if (lc) s += lc->size(); //递归计入左子树规模
    if (rc) s += rc->size(); //递归计入右子树规模
    return s;
}

template<typename T>
BinNodePosi<T> BinNode<T>::insertAsLC(const T &e) {
    return lc = new BinNode(e, this);
}

template<typename T>
BinNodePosi<T> BinNode<T>::insertAsRC(const T &e) {
    return rc = new BinNode(e, this);
}

template <typename T> BinNodePosi<T> BinNode<T>::succ() { //(中序遍历意义下)定位节点v的直接后继
    BinNodePosi<T> s = this; //记录后继的临时变量
    if ( rc ) { //若有右孩子，则直接后继必在右子树中，具体地就是
        s = rc; //右子树中
        while ( HasLChild ( *s ) ) s = s->lc; //最靠左（最小）的节点
    } else { //否则，直接后继应是“将当前节点包含于其左子树中的最低祖先”，具体地就是
        while ( IsRChild ( *s ) ) s = s->parent; //逆向地沿右向分支，不断朝左上方移动
        s = s->parent; //最后再朝右上方移动一步，即抵达直接后继（如果存在）
    }
    return s;
}

template<typename T>
template<typename VST> //元素类型、操作器
void BinNode<T>::travLevel(VST &visit) { //二叉树层次遍历算法
    Queue<BinNodePosi<T>> Q; //辅助队列
    Q.enqueue(this); //根节点入队
    while (!Q.empty()) { //在队列再次变空之前，反复迭代
        BinNodePosi<T> x = Q.dequeue(); //取出队首节点并访问之
        visit(x->data);
        if (HasLChild(*x)) Q.enqueue(x->lc); //左孩子入队
        if (HasRChild(*x)) Q.enqueue(x->rc); //右孩子入队
    }
}

/***********************先序遍历*****************************/
//版本一：迭代版#1
template <typename T, typename VST> //元素类型、操作器
void travPre_I1 ( BinNodePosi<T> x, VST& visit ) {
    Stack<BinNodePosi<T>> S; //辅助栈
    if (x) S.push(x); //根节点入栈
    while (!S.empty()) { //在栈变空之前反复循环
        x = S.pop();
        visit(x->data); //弹出并访问当前节点
        if (HasRChild(*x)) S.push(x->rc);
        if (HasLChild(*x)) S.push(x->lc);
    }
}

//版本二：迭代版#2
//从当前节点出发，沿左分支不断深入，直至没有左分支的节点；沿途节点遇到后立即访问
template <typename T, typename VST> //元素类型、操作器
static void visitAlongVine ( BinNodePosi<T> x, VST& visit, Stack<BinNodePosi<T>>& S ) {
    while ( x ) {
        visit ( x->data ); //访问当前节点
        S.push ( x->rc ); //右孩子入栈暂存（可优化：通过判断，避免空的右孩子入栈）
        x = x->lc;  //沿左分支深入一层
    }
}

template <typename T, typename VST> //元素类型、操作器
void travPre_I2 ( BinNodePosi<T> x, VST& visit ) { //二叉树先序遍历算法（迭代版#2）
    Stack<BinNodePosi<T>> S; //辅助栈
    while ( true ) {
        visitAlongVine ( x, visit, S ); //从当前节点出发，逐批访问
        if ( S.empty() ) break; //直到栈空
        x = S.pop(); //弹出下一批的起点
    }
}

// 版本三：递归版
template<typename T, typename VST>
void travPre_R(BinNodePosi<T> x, VST& visit) {
    if (!x) return;
    visit(x->data);
    travPre_R(x->lc, visit);
    travPre_R(x->rc, visit);
}

//*******先序遍历算法统一入口
template<typename T> template<typename VST> //元素类型、操作器
void BinNode<T>::travPre(VST &visit) {
    switch ( rand() % 3 ) { //此处暂随机选择以做测试，共三种选择
        case 1: travPre_I1 ( this, visit ); break; //迭代版#1
        case 2: travPre_I2 ( this, visit ); break; //迭代版#2
        default: travPre_R ( this, visit ); break; //递归版
    }
}
/***********************先序遍历*****************************/


/***********************中序遍历*****************************/
// 版本一：迭代版#1
template <typename T> //从当前节点出发，沿左分支不断深入，直至没有左分支的节点
static void goAlongVine ( BinNodePosi<T> x, Stack<BinNodePosi<T>>& S ) {
    while ( x ) { S.push ( x ); x = x->lc; } //当前节点入栈后随即向左侧分支深入，迭代直到无左孩子
}

template <typename T, typename VST> //元素类型、操作器
void travIn_I1 ( BinNodePosi<T> x, VST& visit ) {
    Stack<BinNodePosi<T>> S; //辅助栈
    while ( true ) {
        goAlongVine ( x, S ); //从当前节点出发，逐批入栈
        if ( S.empty() ) break; //直至所有节点处理完毕
        x = S.pop(); visit ( x->data ); //弹出栈顶节点并访问之
        x = x->rc; //转向右子树
    }
}

// 版本二：迭代版#2
template <typename T, typename VST> //元素类型、操作器
void travIn_I2 ( BinNodePosi<T> x, VST& visit ) { //二叉树中序遍历算法（迭代版#2）
    Stack<BinNodePosi<T>> S; //辅助栈
    while ( true )
        if ( x ) {
            S.push ( x ); //根节点进栈
            x = x->lc; //深入遍历左子树
        } else if ( !S.empty() ) {
            x = S.pop(); //尚未访问的最低祖先节点退栈
            visit ( x->data ); //访问该祖先节点
            x = x->rc; //遍历祖先的右子树
        } else
            break; //遍历完成
}

// 版本三：迭代版#3
template <typename T, typename VST> //元素类型、操作器
void travIn_I3 ( BinNodePosi<T> x, VST& visit ) { //二叉树中序遍历算法（迭代版#3，无需辅助栈）
    bool backtrack = false; //前一步是否刚从左子树回溯——省去栈，仅O(1)辅助空间
    while ( true )
        if ( !backtrack && HasLChild ( *x ) ) //若有左子树且不是刚刚回溯，则
            x = x->lc; //深入遍历左子树
        else { //否则——无左子树或刚刚回溯（相当于无左子树）
            visit ( x->data ); //访问该节点
            if ( HasRChild ( *x ) ) { //若其右子树非空，则
                x = x->rc; //深入右子树继续遍历
                backtrack = false; //并关闭回溯标志
            } else { //若右子树空，则
                if ( ! ( x = x->succ() ) ) break; //回溯（含抵达末节点时的退出返回）
                backtrack = true; //并设置回溯标志
            }
        }
}

// 版本四：迭代版#4
template <typename T, typename VST> //元素类型、操作器
void travIn_I4 ( BinNodePosi<T> x, VST& visit ) { //二叉树中序遍历（迭代版#4，无需栈或标志位）
    while ( true )
        if ( HasLChild ( *x ) ) //若有左子树，则
            x = x->lc; //深入遍历左子树
        else { //否则
            visit ( x->data ); //访问当前节点，并
            while ( !HasRChild ( *x ) ) //不断地在无右分支处
                if ( ! ( x = x->succ() ) ) return; //回溯至直接后继（在没有后继的末节点处，直接退出）
                else visit ( x->data ); //访问新的当前节点
            x = x->rc; //（直至有右分支处）转向非空的右子树
        }
}

// 版本五：递归版
template <typename T, typename VST> //元素类型、操作器
void travIn_R ( BinNodePosi<T> x, VST& visit ) { //二叉树中序遍历算法（递归版）
    if ( !x ) return;
    travIn_R ( x->lc, visit );
    visit ( x->data );
    travIn_R ( x->rc, visit );
}


template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travIn ( VST& visit ) { //二叉树中序遍历算法统一入口
    switch ( rand() % 5 ) { //此处暂随机选择以做测试，共五种选择
        case 1: travIn_I1 ( this, visit ); break; //迭代版#1
        case 2: travIn_I2 ( this, visit ); break; //迭代版#2
        case 3: travIn_I3 ( this, visit ); break; //迭代版#3
        case 4: travIn_I4 ( this, visit ); break; //迭代版#4
        default: travIn_R ( this, visit ); break; //递归版
    }
}
/***********************中序遍历*****************************/


/***********************后序遍历*****************************/
// 版本一：迭代版
template <typename T> //在以S栈顶节点为根的子树中，找到最高左侧可见叶节点
static void gotoLeftmostLeaf ( Stack<BinNodePosi<T>>& S ) { //沿途所遇节点依次入栈
    while ( BinNodePosi<T> x = S.top() ) //自顶而下，反复检查当前节点（即栈顶）
        if ( HasLChild ( *x ) ) { //尽可能向左
            if ( HasRChild ( *x ) ) S.push ( x->rc ); //若有右孩子，优先入栈
            S.push ( x->lc ); //然后才转至左孩子
        } else //实不得已
            S.push ( x->rc ); //才向右
    S.pop(); //返回之前，弹出栈顶的空节点
}

template <typename T, typename VST>
void travPost_I ( BinNodePosi<T> x, VST& visit ) { //二叉树的后序遍历（迭代版）
    Stack<BinNodePosi<T>> S; //辅助栈
    if ( x ) S.push ( x ); //根节点入栈
    while ( !S.empty() ) { //x始终为当前节点
        if ( S.top() != x->parent ) ////若栈顶非x之父（而为右兄）
            gotoLeftmostLeaf ( S ); //则在其右兄子树中找到HLVFL（相当于递归深入）
        x = S.pop(); visit ( x->data ); //弹出栈顶（即前一节点之后继），并访问之
    }
}

// 版本二：递归版
template <typename T, typename VST> //元素类型、操作器
void travPost_R ( BinNodePosi<T> x, VST& visit ) { //二叉树后序遍历算法（递归版）
    if ( !x ) return;
    travPost_R ( x->lc, visit );
    travPost_R ( x->rc, visit );
    visit ( x->data );
}

template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travPost ( VST& visit ) { //二叉树后序遍历算法统一入口
    switch ( rand() % 2 ) { //此处暂随机选择以做测试，共两种选择
        case 1: travPost_I ( this, visit ); break; //迭代版
        default: travPost_R ( this, visit ); break; //递归版
    }
}
/***********************后序遍历*****************************/

#endif //DSA_BINNODE_H
