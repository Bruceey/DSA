//
// Created by 17634 on 2022/8/31.
//

#ifndef DSA_BTREE_H
#define DSA_BTREE_H

#include "BTNode.h" //引入B-树节点类
#include "../_share/release.h"

template<typename T>
class BTree { //B树模板类
protected:
    int _size; //存放的关键码总数
    int _m; //B-树的阶次，至少为3——创建时指定，一般不能修改
    BTNodePosi<T> _root; //根节点
    BTNodePosi<T> _hot; //BTree::search()最后访问的非空（除非树空）的节点位置
    void solveOverflow(BTNodePosi<T> v); //因插入而上溢之后的分裂处理
    void solveUnderflow(BTNodePosi<T> v); //因删除而下溢之后的合并处理
public:
    BTree( int m = 3 ) : _m(m), _size(0) //构造函数：默认为最低的3阶
    {_root = new BTNode<T>();}
    ~BTree() {if (_root) release(_root);} //析构函数：释放所有节点
    int const order() {return _m;} //阶次
    int const size() {return _size;} //规模
    BTNodePosi<T>& root() {return _root;} //树根
    bool empty() const {return !_root;} //判空
    BTNodePosi<T> search(const T& e); //查找
    bool insert(const T& e); //插入
    bool remove(const T& e); //删除
};

template<typename T>
BTNodePosi<T> BTree<T>::search(const T &e) { //在B-树中查找关键码e
    BTNodePosi<T> v = _root; _hot = nullptr; //从根节点出发
    while (v) { //逐层查找
        Rank r = v->key.search(e); //在当前节点中，找到不大于e的最大关键码
        if ((0 <= r) && (e == v->key[r])) return v; //成功：在当前节点中命中目标关键码
        _hot = v; v = v->child[r + 1]; //否则，转入对应子树（_hot指向其父）——需做I/O，最费时间
    }
    return nullptr; //失败：最终抵达外部节点
}

template<typename T> //关键码插入后若节点上溢，则做节点分裂处理
void BTree<T>::solveOverflow(BTNodePosi<T> v) {
    while (_m <= v->key.size()) { //上溢条件
        Rank s = _m / 2; //轴点（此时应有_m = key.size() = child.size() - 1）
        BTNodePosi<T> u = new BTNode<T>(); //注意：新节点已有一个空孩子
//        for (Rank j = 0; j < _m - s - 1; j++) { //v右侧_m-s-1个孩子及关键码分裂为右侧节点u
//            u->child.insert(j, v->child.remove(s + 1)); //逐个移动效率低
//            u->key.insert(j, v->key.remove(s + 1)); //此策略可改进
//        }
//        u->child[_m - s - 1] = v->child.remove(s + 1);
        u->key = v->key.removeReturnVec(s + 1, v->key.size()); //这两句代替上述代码，效率更高
        u->child = v->child.removeReturnVec(s + 1, v->child.size());

        if (u->child[0]) //若u的孩子们非空，则
            for (Rank j = 0; j < _m - s; j++) //令它们的父节点统一
                u->child[j]->parent = u; //指向u
        BTNodePosi<T> p = v->parent; //v当前的父节点p
        if (!p) {_root = p = new BTNode<T>(); p->child[0] = v; v->parent = p;} //若p空则创建之
        Rank r = 1 + p->key.search(v->key[0]); //p中指向v的指针的秩
        p->key.insert(r, v->key.remove(s)); //轴点关键码上升
        p->child.insert(r + 1, u); u->parent = p; //新节点u与父节点p互联
        v = p; //上升一层，如有必要则继续分裂——至多O(logn)层
    }
}

template<typename T>
bool BTree<T>::insert(const T &e) { //将关键码e插入B树中
    BTNodePosi<T> v = search(e); if (v) return false; //确认目标不存在
    Rank r = _hot->key.search(e); //在节点_hot的有序关键码向量中查找合适的插入位置
    _hot->key.insert(r + 1, e); //将新关键码插至对应的位置
    _hot->child.insert(nullptr); //等同于insert ( r + 2, NULL )，因为最后一次都是空指针
    _size++;
    solveOverflow(_hot); //如有必要，需做分裂
    return true; //插入成功
}

template<typename T> //关键码删除后若节点下溢，则做节点旋转或合并处理
void BTree<T>::solveUnderflow(BTNodePosi<T> v) {
    while ((_m + 1) / 2 > v->child.size()) { //下溢条件
        BTNodePosi<T> p = v->parent;
        if (!p) { //已到根节点，故其父亲为空
            if (!v->key.size() && v->child[0]) { //若树根v不含关键码，却有（唯一的）非空孩子，则
                _root = v->child[0]; _root->parent = nullptr; //这个节点可被跳过
                v->child[0] = nullptr; release(v); //并因不再有用而被销毁
            } //整树高度降低一层
            return;
        }
        Rank r = 0; while (p->child[r] != v) r++; //确定v是p的第r个孩子
    //情况1：向左兄弟借关键码
        if (0 < r) { //若v不是p的第一个孩子，则
            BTNodePosi<T> ls = p->child[r - 1]; //左兄弟必存在
            if ((_m + 1) / 2 < ls->child.size()) { //v的左兄弟阶数大于下限，则顺时针旋转借给v
                v->key.insert(0, p->key[r - 1]); //p借出一个关键码给v（作为最小关键码）
                p->key[r - 1] = ls->key.remove(ls->key.size() - 1); //ls的最大关键码转入p
                v->child.insert(0, ls->child.remove(ls->child.size() - 1)); //同时ls的最右侧孩子过继给v
                if (v->child[0])
                    v->child[0]->parent = v; //然后将其父亲指向v
                return; //至此，通过右旋已完成当前层（以及所有层）的下溢处理
            }
        }
    //情况2：向右兄弟借关键码
        if (r < p->child.size() - 1) { //若v不是p的最后一个孩子，则
            BTNodePosi<T> rs = p->child[r + 1]; //右兄弟必存在
            if ((_m + 1) / 2 < rs->child.size()) { //v的右兄弟阶数大于下限，则逆时针旋转借给v
                v->key.insert(v->key.size(), p->key[r]); //p借出一个关键码给v（作为最大关键码）
                p->key[r] = rs->key.remove(0); //rs的最小关键码转入p
                v->child.insert(v->child.size(), rs->child.remove(0));  //同时rs的最左侧孩子过继给v
                if (v->child[v->child.size() - 1])
                    v->child[v->child.size() - 1]->parent = v; //然后将其父亲指向v
                return; //至此，通过左旋已完成当前层（以及所有层）的下溢处理
            }
        }
    //情况3：左兄弟或者右兄弟不存在，或者左右兄弟都太瘦
        if (0 < r) { //与左兄弟合并
            BTNodePosi<T> ls = p->child[r- 1]; //左兄弟必存在
            ls->key.insert(ls->key.size(), p->key.remove( r- 1)); //剥离父节点秩为r-1的key，以及
            p->child.remove(r); //第r个孩子
            //p的第r - 1个关键码转入ls，p指向v(原为右孩子)的指针已被切断
            //然后将v中key和child并到ls中
            ls->child.insert ( ls->child.size(), v->child.remove ( 0 ) );
            if ( ls->child[ls->child.size() - 1] ) //v的最左侧孩子过继给ls做最右侧孩子
                ls->child[ls->child.size() - 1]->parent = ls;
            while ( !v->key.empty() ) { //v剩余的关键码和孩子，依次转入ls
                ls->key.insert ( ls->key.size(), v->key.remove ( 0 ) );
                ls->child.insert ( ls->child.size(), v->child.remove ( 0 ) );
                if ( ls->child[ls->child.size() - 1] ) ls->child[ls->child.size() - 1]->parent = ls;
            } //该部分效率低，可修改
            release(v); //释放v
        } else { //与有兄弟合并
            BTNodePosi<T> rs = p->child[r + 1]; //右兄弟必存在
            rs->key.insert(0, p->key.remove(r)); //剥离父节点秩为r的key，以及
            p->child.remove(r); //第r个孩子
            //p的第r个关键码转入rs，p指向v(原为左孩子)的指针已被切断
            rs->child.insert(0, v->child.remove(v->child.size() - 1)); //v的最右侧孩子过继给rs，作为最左侧孩子
            if (rs->child[0])
                rs->child[0]->parent = rs;
            while (!v->key.empty()) { //v剩余的关键码和孩子，依次转入rs
                rs->key.insert(0, v->key.remove(v->key.size() - 1));
                rs->child.insert(0, v->child.remove(v->child.size() - 1));
                if (rs->child[0]) rs->child[0]->parent = rs;
            }
            release(v); //释放v
        }
        v = p; //上升一层，如有必要则继续旋转或合并——至多O(logn)层
    } //while
}

template<typename T>
bool BTree<T>::remove(const T &e) { //从BTree树中删除关键码e
    BTNodePosi<T> v = search(e); if (!v) return false; //确认目标关键码存在
    Rank r = v->key.search(e); //确定目标关键码在节点v中的秩（由上，肯定合法）
    if (v->child[0]) { //若v非叶子，则e的后继必属于某叶节点
        BTNodePosi<T> u = v->child[r + 1]; //在右子树中一直向左，即可
        while (u->child[0]) u = u->child[0]; //找出e的后继
        v->key[r] = u->key[0]; v = u; r = 0; //并与之交换位置
    } //至此，v必然位于最底层，且其中第r个关键码就是待删除者
    v->key.remove(r);
    v->child.remove(_size - 1); //外部孩子都是null，故删除最后一个即可；不必删除第r+1个
    _size--;//删除e，以及其下两个外部节点之一
    solveUnderflow(v); ////如有必要，需做旋转或合并
    return true;
}

#endif //DSA_BTREE_H
