//
// Created by 17634 on 2022/8/30.
//

#ifndef DSA_SPLAY_H
#define DSA_SPLAY_H

#include "../BST/BST.h"

template <typename NodePosi> inline //在节点*p与*lc（可能为空）之间建立父（左）子关系
void attachAsLC (NodePosi lc, NodePosi p ) { p->lc = lc; if ( lc ) lc->parent = p; }

template <typename NodePosi> inline //在节点*p与*rc（可能为空）之间建立父（右）子关系
void attachAsRC ( NodePosi p, NodePosi rc ) { p->rc = rc; if ( rc ) rc->parent = p; }

template<typename T>
class Splay : public BST<T> { //由BST派生的Splay树模板类
protected:
    BinNodePosi<T> splay(BinNodePosi<T> v); //将节点v伸展至根
public:
    BinNodePosi<T>& search(const T& e); //查找(重写)
    BinNodePosi<T> insert(const T& e); //插入(重写)
    bool remove(const T& e); //删除(重写)
};

template<typename T> //Splay树伸展算法：从节点v出发逐层伸展
BinNodePosi<T> Splay<T>::splay(BinNodePosi<T> v) { //v为因最近访问而需伸展的节点位置
    if (!v) return nullptr; BinNodePosi<T>p, g; //*v的父亲与祖父
    while ((p = v->parent) && (g = p->parent)) { //自下而上，反复对*v做双层伸展
        BinNodePosi<T> gg = g->parent; //每轮之后*v都以原曾祖父（great-grand parent）为父
        if (IsLChild(*v))
            if (IsLChild(*p)) { //zig-zig
                attachAsLC(p->rc, g); attachAsRC(p, g);
                attachAsLC(v->rc, p); attachAsRC(v, p);
            } else { //zig-zag
                attachAsLC(v->rc, p); attachAsRC(v, p);
                attachAsRC(g, v->lc); attachAsLC(g, v);
            }
        else if (IsRChild(*p)) { //zag-zag
            attachAsRC(g, p->lc); attachAsLC(g, p);
            attachAsRC(p, v->lc); attachAsLC(p, v);
        } else { //zag-zig
            attachAsRC(p, v->lc); attachAsLC(p, v);
            attachAsLC(v->rc, g); attachAsRC(v, g);
        }
        if (!gg) v->parent = nullptr; //若*v原先的曾祖父*gg不存在，则*v现在应为树根
        else //否则，*gg此后应该以*v作为左或右孩子
            (g == gg->lc) ? attachAsLC(v, gg) : attachAsRC(gg, v);
        this->updateHeight(g); //旋转完成后g最多和p平行高度，即zig-zag zag-zig
        this->updateHeight(p); //而zig-zig zag-zag时p高于g
        this->updateHeight(v);
    } //双层伸展结束时，必有g == NULL，但p可能非空
    if (p = v->parent) { //若v最后还有父亲，则额外再做一次单旋
        if (IsLChild(*v)) { attachAsLC(v->rc, p); attachAsRC(v, p);}
        else { attachAsRC(p, v->lc); attachAsLC(p, v);}
        this->updateHeight(p); this->updateHeight(v);
    }
    v->parent = nullptr; return v;
} //调整之后新树根应为被伸展的节点，故返回该节点的位置以便上层函数更新树根

template<typename T>
BinNodePosi<T> &Splay<T>::search(const T &e) { //在伸展树中查找e
    BinNodePosi<T> p = BST<T>::search(e);
    this->_root = splay(p ? p : this->_hot); //将最后一个被访问的节点伸展至根
    return this->_root;
}

template<typename T>
BinNodePosi<T> Splay<T>::insert(const T &e) { //将关键码e插入伸展树中
    if (!this->_root) { this->_size = 1; return this->_root = new BinNode<T>(e);} //原树为空
    BinNodePosi<T> t = search(e); if (e == t->data) return t; //如目标节点t若存在，树根即为结果，不插入
    if (t->data < e) { //在右侧嫁接
        t->parent = this->_root = new BinNode<T> (e, nullptr, t, t->rc);
        if (t->rc) { t->rc->parent = this->_root; t->rc = nullptr; } //若tc->rc不为空，则其父亲指向_root，然后将其置空
    } else { //在左侧嫁接
        t->parent = this->_root = new BinNode<T> (e, nullptr, t->lc, t);
        if (t->lc) {t->lc->parent = this->_root; t->lc = nullptr;}
    }
    this->_size++; this->updateHeightAbove(t);
    return this->_root;
} //无论e是否存在于原树中，返回时总有_root->data == e

template<typename T>
bool Splay<T>::remove(const T &e) { //从伸展树中删除关键码e
    if (!this->_root || (e != search(e)->data)) return false;
    BinNodePosi<T> L = this->_root->lc, R = this->_root->rc; release(this->_root);//记下左、右子树L、R后，释放之
    if (!R) { // 若右子树为空
        if (L) L->parent = nullptr;
        this->_root = L; //L即为余树
    } else { //否则
        this->_root = R; R->parent = nullptr; search(e); //在R中再次查找e：注定失败，但其中的最小节点必
        if (L) L->parent = this->_root;
        this->_root->lc = L; //伸展至根（且无左孩子），故可令其以L作为左子树
    }
    if (--this->_size) this->updateHeight(this->_root);
    return true; //更新规模及树高，报告删除成功
}



#endif //DSA_SPLAY_H
