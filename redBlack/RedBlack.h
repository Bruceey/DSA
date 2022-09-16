//
// Created by 17634 on 2022/9/1.
//

#ifndef DSA_REDBLACK_H
#define DSA_REDBLACK_H

#include "../BST/BST.h"

#define IsBlack(p) ( ! (p) || ( RB_BLACK == (p)->color ) ) //外部节点也视作黑节点
#define IsRed(p) ( ! IsBlack(p) ) //非黑即红
#define BlackHeightUpdated(x) ( /*RedBlack高度更新条件*/ \
   ( stature( (x).lc ) == stature( (x).rc ) ) && \
   ( (x).height == ( IsRed(& x) ? stature( (x).lc ) : stature( (x).lc ) + 1 ) ) \
)

template<typename T> //基于BST实现RedBlack
class RedBlack : public BST<T> { //RedBlack树模板类
protected:
    void solveDoubleRed(BinNodePosi<T> x); //双红修正
    void solveDoubleBlack(BinNodePosi<T> r); //双黑修正
    int updateHeight(BinNodePosi<T> x); //更新节点x的高度（重写）
public:
    BinNodePosi<T> insert(const T& e); //插入（重写）
    bool remove(const T& e); //删除（重写）
    // BST::search()等其余接口可直接沿用
};

template<typename T>
int RedBlack<T>::updateHeight(BinNodePosi<T> x) { //更新节点高度
    return x->height = IsBlack ( x ) + max ( stature ( x->lc ), stature ( x->rc ) ); //孩子黑高度通常相等，除非出现双黑
    /*DSA*/// 红黑树中各节点左、右孩子的黑高度通常相等
    /*DSA*/// 这里之所以取更大值，是便于在删除节点后的平衡调整过程中，正确更新被删除节点父亲的黑高度
    /*DSA*/// 否则，rotateAt()会根据被删除节点的替代者（高度小一）设置父节点的黑高度
}

template<typename T>
BinNodePosi<T> RedBlack<T>::insert(const T &e) { //将e插入红黑树
    BinNodePosi<T> & x = this->search(e); if (x) return x; //确认目标不存在（留意对_hot的设置）
    x = new BinNode<T> (e, this->_hot, nullptr, nullptr, 0); this->_size++; //创建红节点x：以_hot为父，黑高度0
    BinNodePosi<T> xOld = x;
    solveDoubleRed(x); //经双红修正后，即可返回
    return xOld;
}

/******************************************************************************************
 * RedBlack双红调整算法：解决节点x与其父均为红色的问题。分为两大类情况：
 *    RR-1(x的叔叔为黑色)：2次颜色翻转，2次黑高度更新，1~2次旋转，不再递归
 *    RR-2(x的叔叔为红色)：3次颜色翻转，3次黑高度更新，0次旋转，需要递归 O(h)
 ******************************************************************************************/
template<typename T>
void RedBlack<T>::solveDoubleRed(BinNodePosi<T> x) {
    BinNodePosi<T> p = nullptr;
    while (IsRoot(*x) || IsRed(p = x->parent)) {
        if (IsRoot(*x)) { //若已（递归）转至树根，则将其转黑，整树黑高度也随之递增
            this->_root = RB_BLACK;
            this->_root->height++;
            return;
        }
        BinNodePosi<T> g = p->parent; //p为红，x的祖父必然存在，且颜色为黑色
        BinNodePosi<T> u = uncle(x); //以下，视x叔父u的颜色分别处理
        if (IsBlack(u)) { //u为黑色（含NULL）时
            if (IsLChild(*x) == IsLChild(*p)) //若x和p同侧（即zIg-zIg或zAg-zAg），则
                p->color = RB_BLACK; //p由红转黑，x保持红
            else //若x与p异侧（即zIg-zAg或zAg-zIg），则
                x->color = RB_BLACK; //x由红转黑，p保持红
            g->color = RB_RED; //g必定由红转黑
            BinNodePosi<T> gg = g->parent; // 曾祖父
            BinNodePosi<T> r = FromParentTo(*g) = this->rotateAt(x); //调整后的子树根节点
            r->parent = gg; //与原曾祖父联接
            return; //不再向上传递，直接返回
        } else {
            p->color = RB_BLACK;
            p->height++; //p由红转黑
            u->color = RB_BLACK;
            u->height++; //u由红转黑
            g->color = RB_RED; //在B-树中g相当于上交给父节点的关键码，故暂标记为红
            x = g; //继续调整：若已至树根，接下来的递归会将g转黑（尾递归，不难改为迭代）
        }
    }
}

template<typename T>
bool RedBlack<T>::remove(const T &e) { //从红黑树中删除关键码e
    BinNodePosi<T> & x = this->search(e); if (!x) return false; //确认目标存在（留意_hot的设置）
    BinNodePosi<T> r = removeAt(x, this->_hot); //此时_hot指向r的父亲
    if (!(--this->_size)) return true; //删除后_size-1，若全树节点为空，则直接退出(这里仅仅是简化处理，没有更新树空时的高度)
    if (!this->_hot) { //若刚被删除的是根节点(此时其父亲_hot为空)，
        this->_root->color = RB_BLACK; //则将其置黑，
        updateHeight(this->_root); //并更新根节点黑高度
        return true;
    }
// assert: 以下，原x（现r）必非根，_hot必非空
    //TODO 这个宏有点费解
    if (BlackHeightUpdated(*this->_hot)) return true; //若r的父亲黑深度不变，则无需调整
    if (IsRed(r)) { //否则，若r为红，则只需令其转黑
        r->color = RB_BLACK; r->height++; return true;
    }
// assert: 以下，原x（现r）均为黑色，其父亲p、兄弟s必然存在
    solveDoubleBlack(r); //经双黑调整后返回
    return true;
}

template<typename T>
void RedBlack<T>::solveDoubleBlack(BinNodePosi<T> r) {
    //此处不能直接写_hot,因为有递归，发生递归时只能写r->parent;
    // 当传进来时r可能是外部节点，所以要用_hot表示其父亲
    BinNodePosi<T> p = r ? r->parent : this->_hot;
    if ( !p ) return;
    BinNodePosi<T> s = (r == p->lc) ? p->rc : p->lc; //r的兄弟
    if (IsBlack(s)) { //黑s有红孩子：BB-1
        BinNodePosi<T> t = nullptr; //s的红孩子（若左、右孩子皆红，左者优先；皆黑时为NULL）
        if (IsRed(s->rc)) t = s->rc;
        if (IsRed(s->lc)) t = s->lc; //左者优先
        if (t) { //黑s有红孩子：BB-1
            RBColor oldColor = p->color; //记录原先节点的颜色
            BinNodePosi<T> b = FromParentTo(*p) = this->rotateAt(t); //旋转
            if (HasLChild(*b)) {b->lc->color = RB_BLACK; updateHeight(b->lc);}
            if (HasRChild(*b)) {b->rc->color = RB_BLACK; updateHeight(b->rc);}
            b->color = oldColor; //新子树根节点继承原根节点的颜色
            updateHeight(b);
        } else { //黑s无红孩子 BB-2R和BB-2B两种情况
            s->color = RB_RED; s->height--; //s转红
            if (IsRed(p)) { //BB-2R
                p->color = RB_BLACK; //p转黑，但黑高度不变
            } else { //BB-2B
                p->height--; //p保持黑，但高度下降
                solveDoubleBlack(p); //递归上溯
            }
        }
    } else { //兄弟s为红：BB-3
        s->color = RB_BLACK, p->color = RB_RED; //s转黑，p转红
        BinNodePosi<T> t = IsLChild(*s) ? s->lc : s->rc; //取t与其父同侧
        this->_hot = p;
        FromParentTo(*p) = this->rotateAt(t); //对t及其父亲、祖父做平衡调整
        solveDoubleBlack(r); //继续修正r处双黑——此时的p已转红，故后续只能是BB-1或BB-2R
    }
}
//若目标节点存在且被删除，返回true；否则返回false

#endif //DSA_REDBLACK_H
