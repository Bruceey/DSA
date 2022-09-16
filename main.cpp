//#include <iostream>
//#include "vector/Vector.h"
//#include "list/List.h"
//#include "stack/Stack.h"
//#include "queue/Queue.h"
//#include "graph/Graph.h"
//#include "AVL/AVL.h"
//using namespace std;
//
//
//int main() {
////    // 1.Vector测试
////    int A[] = {1,19,10,5,-4,-6,-17,12};
////    Vector<int> v(A, sizeof(A) / sizeof(A[0]));
////    v.show();
////    v.sort();
////    v.show();
//
////    // 2.List测试
////    int A[] = {1,19,10,5,-4,-6,-17,12};
////    List<int> L;
////    for (int & i : A)
////        L.insertAsLast(i);
////    L.show();
//////    cout << L.disordered();
//////    L.sort();
////    L.reverse();
////    L.show();
//
////    // 3.栈测试
////    int A[] = {1,19,10,5,-4,-6,-17,12};
////    Stack<int> S;
////    for (int& e : A)
////        S.push(e);
////    S.show();
////    for (int i = 0; i < 8; i++) {
////        cout << S.pop() << " ";
////    }
//
////    // 4.队列测试
////    int A[] = {1,19,10,5,-4,-6,-17,12};
////    Queue<int> Q;
////    for (int& e : A)
////        Q.enqueue(e);
////    Q.show();
////    for (int i = 0; i < 8; i++) {
////        cout << Q.dequeue() << " ";
////    }
//
//    return 0;
//}
// 测试

#include<iostream>
using namespace std;
class Base{
public:
    Base(int c=10){cout<< "基类带参构造函数" << c << endl;}
    ~Base(){cout<<"基类析构" << endl;}
};
class Derived:public Base{
public:
    Derived(int c) // 调用基类构造函数
    {
        cout<< "派生类带参构造函数" << c << endl;
    }
    ~Derived(){cout<<"派生类析构" << endl;}
};

int main()
{
    int i = 9;
    Derived d1(i);

    return 0;
}