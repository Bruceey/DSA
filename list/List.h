//
// Created by 王锐 on 2021/11/3.
// 注意：列表的search接口中的n个节点不包含p，其他函数的n个节点均包含p
//

#ifndef DSA_LIST_H
#define DSA_LIST_H

#include "ListNode.h"
#include <cstdlib>
#include <iostream>

template<typename T>
class List {    //列表模版类
private:
    int _size;
    ListNodePosi<T> header;
    ListNodePosi<T> trailer;  //规模、头哨兵、尾哨兵
protected:
    void init();    //列表创建时的初始化
    int clear();    //清除所有节点
    void copyNodes(ListNodePosi<T> &, int);  //复制列表自位置p起的n项
    void merge(ListNodePosi<T> &, int, List<T> &, ListNodePosi<T>, int); //归并
    void mergeSort(ListNodePosi<T> &, int); //对从p开始连续的n个节点归并排序
    void selectionSort(ListNodePosi<T>, int); //对从p开始连续的n个节点选择排序
    void insertionSort(ListNodePosi<T>, int); //对从p开始连续的n个节点插入排序
public:
    // 构造函数
    List() { init(); }
    List(List<T> const &L);  //整体复制列表L
    List(List<T> const &L, Rank r, int n); //复制列表L中自第r项起的n项
    List(ListNodePosi<T> p, int n); //复制列表中自位置p起的n项

    //析构函数
    ~List(); //释放（包含头、尾哨兵在内的）所有节点

    //只读访问接口
    Rank size() const { return _size; }   //规模
    bool empty() const { return _size <= 0; } //判空
    T& operator[](Rank r) const; //重载，支持循秩访问（效率低）
    ListNodePosi<T> first() const { return header->succ; } //首节点位置
    ListNodePosi<T> last() const { return trailer->pred; } //尾节点位置
    bool valid(ListNodePosi<T> p)  //判断位置p是否对外合法
    { return p && (trailer != p) && (header != p); }
    int disordered() const; //判断列表是否已排序

    ListNodePosi<T> find(T const &e) const//无序列表查找
    { return find(e, _size, trailer); }
    ListNodePosi<T> find(T const &e, int n, ListNodePosi<T> p) const; //在无序列表内节点p的n个前驱中，查找等于e的最后者

    ListNodePosi<T> search(T const &e) const     //有序列表查找
    { return search(e, _size, trailer); }
    ListNodePosi<T> search(T const &e, int n, ListNodePosi<T> p) const; //在有序列表内节点p（可能是trailer）的n个前驱中，找到不大于e的最后者

    ListNodePosi<T> selectMax(ListNodePosi<T> p, int n); //在p及其n-1个后继中选出最大值
    ListNodePosi<T> selectMax() { return selectMax(header->succ, _size); }  //整体最大值

//可写访问接口
    ListNodePosi<T> insertAsFirst(T const &e); //将e作为首节点插入
    ListNodePosi<T> insertAsLast(T const &e);  //将e作为尾节点插入
    ListNodePosi<T> insert(ListNodePosi<T> p, T const &e); //将e作为p的后继插入
    ListNodePosi<T> insert(T const &e, ListNodePosi<T> p); //将e作为p的前驱插入
    T remove(ListNodePosi<T> p); //删除合法位置p处的节点，返回被删除节点
    void merge(List<T> &L)      //全列表归并
    { merge(header->succ, _size, L, L.header->succ, L._size); }

    void sort(ListNodePosi<T> p, int n); //列表区间排序
    void sort() { sort(first(), _size); }  //列表整体排序
    int deduplicate(); //无序去重
    int uniquify(); //有序去重
    void reverse(); //前后倒置

//遍历
    void traverse(void (* )(T &)); //遍历，以此实施visit操作(函数指针，只读或局部修改)
    template<typename VST> //操作器
    void traverse(VST &);  //遍历，以此实施visit操作（函数对象，可全局性修改）

    void show() const {
        std::cout << "List<[";
        ListNodePosi<T> p = header;
        while ((p = p->succ) != trailer) {
            if (p != trailer->pred)
                std::cout << p->data << " ";
            else
                std::cout << p->data;
        }
        std::cout << "]>\n";
    }
}; //List



/******************************************************************************************
 * List列表类的相关实现
 ******************************************************************************************/
template<typename T>
void List<T>::init() {
    header = new ListNode<T>;  //创建头哨兵节点
    trailer = new ListNode<T>; //创建尾哨兵节点
    header->succ = trailer;
    header->pred = nullptr;
    trailer->pred = header;
    trailer->succ = nullptr;
    _size = 0;
}

template<typename T>
//重载下标运算符，通过秩访问
T &List<T>::operator[](Rank r) const { // assert: 0<= r < _size
    ListNodePosi<T> p = first(); //从首节点出发
    while (r--) p = p->succ;
    return p->data;
}

// ************查找************
//无序列表查找
template<typename T> //在无序列表内节点p的n个前驱中，查找等于e的最后者
ListNodePosi<T> List<T>::find(T const &e, int n, ListNodePosi<T> p) const {
    while (n--)
        if (e == (p = p->pred)->data) return p;
    return nullptr; //查找不到返回空指针
}


//************插入*************
template<typename T>
ListNodePosi<T> List<T>::insertAsFirst(T const &e) {    //e当作首元素插入
    _size++;
    return header->insertAsSucc(e);
}

template<typename T>
ListNodePosi<T> List<T>::insertAsLast(T const &e) {    //e作为尾元素插入
    _size++;
    return trailer->insertAsPred(e);
}

template<typename T>
ListNodePosi<T> List<T>::insert(ListNodePosi<T> p, T const &e) {
    _size++;
    return p->insertAsSucc(e);
}//将e作为p的后继插入

template<typename T>
ListNodePosi<T> List<T>::insert(T const &e, ListNodePosi<T> p) {
    _size++;
    return p->insertAsPred(e);
}//将e作为p的前驱插入

//**********复制构造
template<typename T>
//复制列表自位置p起的n项
void List<T>::copyNodes(ListNodePosi<T> &p, int n) {  //p合法，且至少有n-1个后继节点
    init(); //创建头尾哨兵并做初始化
    while (n--) {
        insertAsLast(p->data);
        p = p->succ;
    }
}

template<typename T>
List<T>::List(List<T> const &L) { //整体复制列表L
    copyNodes(L.first(), L._size);
}

template<typename T>
List<T>::List(List<T> const &L, Rank r, int n) {  //复制列表L中自第r项起的n项, assert: r+n <= L._size
    copyNodes(L[r], n);
}

template<typename T>
List<T>::List(ListNodePosi<T> p, int n) { //复制列表中自位置p起的n项
    copyNodes(p, n);
}

//*******删除*********
template<typename T>
T List<T>::remove(ListNodePosi<T> p) {   //删除合法位置p处的节点，返回被删除节点
    T e = p->data;  //备份
    p->pred->succ = p->succ;
    p->succ->pred = p->pred; // 后继、前驱
    delete p;
    _size--;  //释放节点、更新规模
    return e; //返回备份的数值
}


//*******析构函数**********
template<typename T>
List<T>::~List() {
    clear();
    delete header;
    delete trailer; //清空列表、释放头、尾哨兵节点
//    std::cout << "All ListNodes have been deleted...\n";
    std::cout << "\n列表中所有节点全部清除了。。。\n";
}

template<typename T>
int List<T>::clear() {
    int oldSize = _size;
    while (0 < _size) remove(header->succ);
    return oldSize;
}

//无序列表唯一化
template<typename T>
int List<T>::deduplicate() { //剔除无序列表中的重复元素，复杂度O(n^2)
    if (_size < 2) return 0;   //平凡列表无重复
    int oldSize = _size;        //记录原始规模
    ListNodePosi<T> p = header;
    Rank r = 0; //p从首节点开始
    while (trailer != (p = p->succ)) {
        ListNodePosi<T> q = find(p->data, r, p);
        q ? remove(q) : r++;
        return oldSize - _size;
    }
}


//遍历
template<typename T>
void List<T>::traverse(void (*visit)(T &)) {  //函数指针
    for (ListNodePosi<T> p = header->succ; p != trailer; p = p->succ) visit(p->data);
}

template<typename T>
template<typename VST>
// 元素类型、操作器
void List<T>::traverse(VST &visit) {  //函数对象
    for (ListNodePosi<T> p = header->succ; p != trailer; p = p->succ) visit(p->data);
}


//有序列表唯一化
template<typename T>
int List<T>::uniquify() {
    if (_size < 2) return 0;
    int oldSize = _size;
    ListNodePosi<T> p = first();
    ListNodePosi<T> q;  //p为各区段起点，q为后继
    while (trailer != (q = p->succ)) {
        if (p->data != q->data) p = q;
        else remove(q);
    }
    return oldSize - _size;
}


// 有序列表查找
template<typename T>
ListNodePosi<T> List<T>::search(const T &e, int n, ListNodePosi<T> p) const {  //在有序列表内节点p（可能是trailer）的n个前驱中，找到不大于e的最后者
    // assert: 0<= n <= rank(p) <_size
    while (0 <= n--)
        if (((p = p->pred)->data) <= e) break; //最后到header处，不管<=e否、都返回header
    return p;
}


//*******排序器***********
template<typename T>
//列表区间排序
void List<T>::sort(ListNodePosi<T> p, int n) {
//    switch (rand() % 3) {
//        case 1:
//            insertionSort(p, n);
//            break;
//        case 2:
//            selectionSort(p, n);
//            break;
//        default:
//            mergeSort(p, n);
//            break;
//    }
    mergeSort(p, n);
}

// 插入排序
template<typename T>
void List<T>::insertionSort(ListNodePosi<T> p, int n) { // valid(p) && rank(p) + n <= _size
    for (int r = 0; r < n; r++) {
        insert(search(p->data, r, p), p->data); //查找适当的位置并插入
        p = p->succ;
        remove(p->pred);
    }
}

// 选择排序
template<typename T>
void List<T>::selectionSort(ListNodePosi<T> p, int n) { // valid(p) && rank(p) + n <= _size
    ListNodePosi<T> head = p->pred;
    ListNodePosi<T> tail = p;
    for (int i = 0; i < n; i++) tail = tail->succ;  //排序区间为(head, tail)
    while (1 < n) {  //至少两个元素
        ListNodePosi<T> max = selectMax(head->succ, n);
        insert(remove(max), tail);
        tail = tail->pred;
        n--;
    }
}

template<typename T>
//从起始于位置p的n个元素中选出最大者
ListNodePosi<T> List<T>::selectMax(ListNodePosi<T> p, int n) {
    ListNodePosi<T> max = p;
    for (ListNodePosi<T> cur = p; 1 < n; n--)
        if (max->data <= (cur = cur->succ)->data) max = cur;
    return max;
}

//归并排序
template<typename T> //列表的归并排序算法：对起始于位置p的n个元素排序
void List<T>::mergeSort(ListNodePosi<T> &p, int n) { //valid(p) && rank(p) + n <= size
    if (n < 2) return; //若待排序范围已足够小，则直接返回；否则...
    int m = n >> 1; //以中点为界
    ListNodePosi<T> q = p;
    for (int i = 0; i < m; i++) q = q->succ; //找到后子列表起点
    mergeSort(p, m); //前、后子列表各分别排序
    mergeSort(q, n - m);
    merge(p, m, *this, q, n - m); //归并
} //注意：排序后，p依然指向归并后区间的（新）起点

template<typename T> //有序列表的归并：当前列表中自p起的n个元素，与列表L中自q起的m个元素归并
void List<T>::merge(ListNodePosi<T> &p, int n, List<T> &L, ListNodePosi<T> q, int m) {
// assert:  this.valid(p) && rank(p) + n <= size && this.sorted(p, n)
//          L.valid(q) && rank(q) + m <= L._size && L.sorted(q, m)
// 注意：在被mergeSort()调用时，this == &L && rank(p) + n = rank(q)
    ListNodePosi<T> pp = p->pred; //归并之后p可能不再指向首节点，故需先记忆，以便在返回前更新
    while (0 < m && p != q) { //q尚未出界（或在mergeSort()中，p亦尚未出界）之前
        if ((0 < n) && p->data <= q->data) { //若p尚未出界且v(p) <= v(q)，则
            p = p->succ; //p直接后移，即完成归入
            n--;
        } else { //否则，将q转移至p之前，以完成归入
            insert(L.remove((q = q->succ)->pred), p);
            m--;
        }
    }
    p = pp->succ; //更新的首节点
}

template<typename T>
int List<T>::disordered() const { //判断列表是否已排序，返回逆序对数
    ListNodePosi<T> p = header;
    int n = 0;
    for (Rank r = 0; r < _size - 1; r++) {
        p = p->succ;
        if (p->data > p->succ->data) n++;
    }
    return n;
}

template<typename T>
void List<T>::reverse() { //前后倒置
    ListNodePosi<T> p = header;
    ListNodePosi<T> q = trailer; //头、尾节点
    for (int i = 1; i < _size; i += 2) //（从首、末节点开始）由外而内，成对地
        swap((p = p->succ)->data, (q = q->pred)->data); //交换对称节点的数据项
}

#endif //DSA_LIST_H
