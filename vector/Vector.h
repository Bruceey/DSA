//
// Created by 王锐 on 2021/11/1.
//

#ifndef DSA_VECTOR_H
#define DSA_VECTOR_H

#include "Fib.h"
#include <iostream>

#include <cstdlib>
using namespace std;

typedef int Rank;       //秩
#define DEFAULT_CAPACITY 3      // 默认的初始容量

template<typename T>
class Vector {          // 向量模版类
protected:
    Rank _size;
    int _capacity;
    T *_elem;    // 规模、容量、数据区
    void copyFrom(T const *A, Rank lo, Rank hi);   //复制数组区间A[lo, hi)
    void expand();      // 空间不足时扩容使用
    void shrink();      //装填因子过小时压缩
    bool bubble(Rank lo, Rank hi);  //扫描交换
    void bubbleSort(Rank lo, Rank hi);  //起泡排序算法
    Rank maxItem(Rank lo, Rank hi); //选取最大元素
    void selectionSort(Rank lo, Rank hi);   //选择排序算法
    void merge(Rank lo, Rank mi, Rank hi);  //归并算法
    void mergeSort(Rank lo, Rank hi);   //归并排序算法
    void heapSort(Rank lo, Rank hi);    //堆排序算法
    Rank partition(Rank lo, Rank hi);   //轴点构造算法
    void quickSort(Rank lo, Rank hi);   //快速排序算法
public:
    //构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {     //容量为c，规模为s，所有元素初始为v
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v);
        cout << _elem << "创建向量。。\n";
    }
    Vector(T const *A, Rank n) { copyFrom(A, 0, n); }   //数组整体复制
    Vector(T const *A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }    // 区间复制
    Vector(Vector<T> const &V) { copyFrom(V._elem, 0, V._size); }    // 向量整体复制
    Vector(Vector<T> const &V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); }    // 区间复制

    //析构函数
    ~Vector() { std::cout << _elem << "向量中所有元素正在删除...\n"; delete[] _elem;}

    // 只读访问接口
    Rank size() const { return _size; }   // 规模
    bool empty() const { return !_size; }     //判空
    int disordered() const;     //判断向量是否已排序
    Rank find(T const &e) const { return find(e, 0, _size); }     //无序向量整体查找
    Rank find(T const &e, Rank lo, Rank hi) const;      //无序向量区间查找
    Rank search(T const &e) const {      //有序向量整体查找
        return (0 >= _size) ? -1 : search(e, 0, _size);
    }
    Rank search(T const &e, Rank lo, Rank hi) const;    //有序向量区间查找

    // 可写访问接口
    T& operator[](Rank r) const;   //重载下标操作符，可以类似于数组形式引用各元素
    Vector<T> &operator=(Vector<T> const &V);  //重载赋值操作符，以便直接克隆向量
    T remove(Rank r);   //删除秩为r的元素
    int remove(Rank lo, Rank hi);    //删除秩在区间[lo, hi)之内的元素
    Vector<T> removeReturnVec(Rank lo, Rank hi); //删除秩在区间[lo, hi)之内的元素，并将其以新的向量形式返回; 专为B树分裂使用
    Rank insert(Rank r, T const &e);    //插入元素
    Rank insert(T const &e) { return insert(_size, e); }    //默认作为末元素插入
    void sort(Rank lo, Rank hi);    //对[lo, hi)排序
    void sort() { sort(0, _size); }     //对整体排序
    void unsort(Rank lo, Rank hi);      // 对[lo, hi)置乱
    void unsort() { unsort(0, _size); }     //对整体置乱
    int deduplicate();  //无序去重
    int uniquify();     //有序去重

    //遍历
    void traverse(void (*visit)(T &));      //遍历（使用函数指针，只读或局部修改）
    template<typename VST>
    void traverse(VST &visit); // 遍历（使用函数对象，可全局性修改）

    // Vector类的字符串显示
    void show() {
        cout << "Vector<[";
        for (int i = 0; i < _size; ++i) {
            if (i != _size - 1) cout << _elem[i] << " ";
            else cout << _elem[i];
        }
        cout << "]>\n";
    }
};  //Vector




/******************************************************************************************
 * Vector向量类的相关实现
 ******************************************************************************************/

template<typename T>
void permute(Vector<T> &V) { //随机置乱向量，使各元素等概率出现于各位置
    for (int i = V.size(); i > 0; i--) { //自后向前
        swap(V[i - 1], V[rand() % i]); //V[i - 1]与V[0, i)中某一随机元素交换
    }
}

template<typename T>
void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi) {
    cout << _elem << "创建向量。。\n";
    _elem = new T[_capacity = 2 * (hi - lo)];
    _size = 0;    //分配空间，规模清零
    while (lo < hi)
        _elem[_size++] = A[lo++];
}

template<typename T>
void Vector<T>::expand() {      //向量空间不足时扩容
    if (_size < _capacity) return;  //尚未满员时，不必扩容
    if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY; //不低于最小容量
    T *oldElem = _elem;
    _elem = new T[_capacity <<= 1];     //容量加倍
    for (int i = 0; i < _size; i++) //复制原向量内容
        _elem[i] = oldElem[i];
    delete[] oldElem;
}

template<typename T>
void Vector<T>::shrink() {      //装填因子过小时压缩向量所占空间
    if (_capacity < DEFAULT_CAPACITY << 1) return;  //不致收缩到DEFAULT_CAPACITY以下
    if (_size << 2 > _capacity) return;     //以25%为界
    T *oldElem = _elem;
    _elem = new T[_capacity >>= 1];  //容量减半
    for (int i = 0; i < _size; i++)   //复制原向量内容
        _elem[i] = oldElem[i];
    delete[] oldElem;
}

template<typename T>
bool Vector<T>::bubble(Rank lo, Rank hi) { //一趟扫描交换，使得最大值沉入湖底
    bool sorted = true; //整体有序标志
    while (++lo < hi)  //自左向右，逐一检查各对相邻元素
        if (_elem[lo - 1] > _elem[lo]) { // 若逆序
            sorted = false;
            swap(_elem[lo - 1], _elem[lo]);
        }
    return sorted;
}

template<typename T>
void Vector<T>::bubbleSort(Rank lo, Rank hi) { //起泡排序
    while (!bubble(lo, hi--)); //逐趟扫描，直至全部有序
}

template<typename T>
Rank Vector<T>::maxItem(Rank lo, Rank hi) { //在[lo, hi]内找出最大者
    Rank mx = hi;
    while (lo < hi--) //逆向扫描，选取最大的索引
        if (_elem[hi] > _elem[mx])
            mx = hi;
    return mx;
}

template<typename T>
void Vector<T>::selectionSort(Rank lo, Rank hi) { //选择排序
    while (lo < --hi)
        swap(_elem[maxItem(lo, hi)], _elem[hi]); //将[hi]与[lo, hi]中的最大者交换
}

template<typename T>
void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    Rank i = 0;
    T *A = _elem + lo;
    Rank j = 0, lb = mi - lo;
    T B[lb];
    for (int i = 0; i < lb; i++) B[i] = A[i];
    int k = 0, lc = hi - mi;
    T *C = _elem + mi;
    while (j < lb && k < lc)
        A[i++] = (B[j] <= C[k]) ? B[j++] : C[k++];
    // 如果C先取完，将B中剩余的接到后面
    while (j < lb) A[i++] = B[j++];
}

template<typename T>
void Vector<T>::mergeSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    int mi = (lo + hi) / 2;
    mergeSort(lo, mi);
    mergeSort(mi, hi);
    merge(lo, mi, hi);
}

template<typename T>
Rank percolateDown(T *A, Rank n, Rank i) { //对向量第i个元素实施下滤，i < n
    Rank j; //假设j为左右孩子的最大值对应的索引
    while ((j = 2 * i + 1) < n) {
        if (j + 1 < n && A[j] < A[j + 1]) j += 1; //和右孩子比较、更新索引

        if (A[i] < A[j]) { //父节点和子节点比较更新
            swap(A[i], A[j]);
            i = j;
        } else break;
    }
    return i; // 返回下滤抵达的位置
}

template<typename T>
void heapify(T *A, Rank n) {
    for (Rank i = n / 2 - 1; 0 <= i; i--) {
        percolateDown(A, n, i);
    }
}

template<typename T>
void Vector<T>::heapSort(Rank lo, Rank hi) {
    T *A = _elem + lo;
    Rank n = hi - lo;
    heapify(A, n);
    while (0 < --n) {
        swap(A[0], A[n]);
        percolateDown(A, n, 0);
    }
}

template<typename T>
Rank Vector<T>::partition(Rank lo, Rank hi) {
    swap(_elem[lo], _elem[lo + rand() % (hi - lo)]); //任一元素与首元素交换
    T pivot = _elem[lo];
    int mi = lo;

    for (Rank k = lo + 1; k < hi; k++)
        if (_elem[k] < pivot)
            swap(_elem[++mi], _elem[k]);
    swap(_elem[lo], _elem[mi]); //候选轴点归位
    return mi; //返回轴点的秩
}

template<typename T>
void Vector<T>::quickSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return; //单元素自然有序
    Rank mi = partition(lo, hi); //在[lo, hi)内构造轴点
    quickSort(lo, mi); //对前缀递归排序
    quickSort(mi + 1, hi); //对后缀递归排序
}

template<typename T>
int Vector<T>::disordered() const {     //返回向量中逆序相邻元素对的总数
    int n = 0;  //计数器
    for (int i = 1; i < _size; i++)
        if (_elem[i - 1] > _elem[i]) n++;     //逆序则计数
    return n;   //向量有序当且仅当n = 0
}

template<typename T>
// 无序向量的顺序查找：返回最后一个元素e的位置；失败时，返回lo - 1
Rank Vector<T>::find(const T &e, Rank lo, Rank hi) const {    // assert: 0<= lo < hi <= _size
    while ((lo < hi--) && (e != _elem[hi]));
    return hi;
}

//******************************
// 有序向量查找

// 二分查找（版本A）: 在有序向量区间[lo, hi)查找e，0 <= lo <= hi <= _size
template<typename T>
// 平均查找时间O(1.5logn)
static Rank binSearch_A(T *A, T const &e, Rank lo, Rank hi) {
    while (lo < hi) {
        Rank mi = (lo + hi) / 2; //以中点为轴点
        if (e < A[mi]) hi = mi;
        else if (A[mi] < e) lo = mi + 1;
        else return mi;
    }
    return -1;  //查找失败返回-1
}

// Fabonacci查找 ,平均查找时间O(1.44logn)
template<typename T>
static Rank fibSearch(T *A, T const &e, Rank lo, Rank hi) {
    Fib fib(hi - lo);
    while (lo < hi) {
        while (hi - lo < fib.get()) fib.prev();
        Rank mi = lo + fib.get() - 1;
        if (e < A[mi]) hi = mi;
        else if (A[mi] < e) lo = mi + 1;
        else return mi;
    }
    return -1; //查找失败返回-1
}

// 二分查找版本B
template<typename T>
static Rank binSearch_B(T *A, T const &e, Rank lo, Rank hi) {
    while (1 < hi - lo) {
        Rank mi = (lo + hi) >> 1;
        (e < A[mi]) ? hi = mi : lo = mi;
    }
    return (e == A[lo]) ? lo : -1; //查找失败返回-1
}

// 二分查找版本C，返回最后一个不大于e的最大秩
template<typename T>
static Rank binSearch_C(T *A, T const &e, Rank lo, Rank hi) {
    while (lo < hi) {
        Rank mi = (lo + hi) >> 1;
        (e < A[mi]) ? hi = mi : lo = mi + 1;
    }
    return --lo;
}

template<typename T>
Rank Vector<T>::search(const T &e, Rank lo, Rank hi) const {
    return binSearch_C(_elem, e, lo, hi);
}
//******************************

template<typename T>
T& Vector<T>::operator[](Rank r) const {      //重载下标操作符
    return _elem[r];
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T> &V) {
    if (this != &V) {
        if (_elem) delete[] _elem;   // 释放原有内容
        copyFrom(V._elem, 0, V._size); // 整体复制
    }
    return *this;
}

template<typename T>
T Vector<T>::remove(Rank r) {
    T e = _elem[r]; //备份
    remove(r, r + 1);
    return e;   //返回删除元素
}

template<typename T>
int Vector<T>::remove(Rank lo, Rank hi) { //删除区间[lo, hi)
    if (lo == hi) return 0;     //出于效率考虑，单独处理退化情况（即删除单个元素）
    while (hi < _size) _elem[lo++] = _elem[hi++];
    _size = lo; //更新规模
    shrink();  //若有必要，缩容
    return hi - lo; //返回被删除元素的个数
}

template<typename T> //删除自身秩在区间[lo, hi)之内的元素，并将其以新的向量形式返回
Vector<T> Vector<T>::removeReturnVec(Rank lo, Rank hi) { //专为B树分裂使用
    if (lo == hi) return NULL;
    Vector<T> V = Vector<T>(*this, lo, hi);
    _size -= V._size;
    shrink();
    return V;
}

template<typename T>
Rank Vector<T>::insert(Rank r, const T &e) {  // assert: 0 <= r <= _size
    expand();       //若有必要，扩容
    for (int i = _size; i > r; i--) _elem[i] = _elem[i - 1];
    _elem[r] = e;
    _size++;
    return r;
}

template<typename T>
void Vector<T>::sort(Rank lo, Rank hi) {
//    mergeSort(lo, hi);
    heapSort(lo, hi);
}

template<typename T>
void Vector<T>::unsort(Rank lo, Rank hi) {
    T *V = _elem + lo;  //将子向量_elem[lo, hi)视作另一向量V[0, hi-lo)
    for (Rank i = hi - lo; i > 0; i--) {    //自后向前
        swap(V[i - 1], V[rand() % i]);      //将V[i-1]与V[0, i)中某一元素随机交换
    }
}

template<typename T>
int Vector<T>::deduplicate() {      //删除无序向量中的重复元素，复杂度n^2
    int oldSize = _size;    //记录原规模
    Rank i = 1;
    while (i < _size)
        find(_elem[i], 0, i) < 0 ?     //在其前缀中寻找与之雷同者（至多一个）
        i++ : remove(i);            //若无雷同继续考察后继，否则删除雷同者
    return oldSize - _size;         //向量变化规模
}

template<typename T>
int Vector<T>::uniquify() { //有序去重，区间 删除
    Rank i = 0, j = 0;  //各队互异"相邻"元素的值
    while (++j < _size) // 逐一扫描，直至末元素
        if (_elem[i] != _elem[j])   //跳过相同者
            _elem[++i] = _elem[j];  //发现不同元素时，向前移至紧邻前者右侧
    _size = ++i;
    shrink();    //必要时缩容
    return j - i;   //向量规模变化量，即被删除元素总数；复杂度O(n)
}

template<typename T>
void Vector<T>::traverse(void (*visit)(T &)) {      //借助函数指针
    for (int i = 0; i < _size; i++) visit(_elem[i]);    //遍历向量
}

template<typename T>
template<typename VST>
void Vector<T>::traverse(VST &visit) {   //借助函数对象
    for (int i = 0; i < _size; i++) visit(_elem[i]);
}

#endif //DSA_VECTOR_H