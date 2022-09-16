//
// Created by 王锐 on 2021/11/7.
//

#ifndef DSA_GRAPH_H
#define DSA_GRAPH_H

#include <climits>
#include "../stack/Stack.h"
#include "../queue/Queue.h"
using namespace std;

using VStatus = enum { UNDISCOVERED, DISCOVERED, VISITED };  //顶点状态
using EType = enum { UNDETERMINED, TREE, CROSS, FORWARD, BACKWARD }; //边在遍历树中所属的类型

template <typename Tv, typename Te> //顶点类型、边类型
class Graph { //图Graph抽象模板类
private:
    void reset() { //所有顶点、边的辅助信息复位
        for ( Rank v = 0; v < n; v++ ) { //所有顶点的
            status ( v ) = UNDISCOVERED; dTime ( v ) = fTime ( v ) = -1; //状态，时间标签
            parent ( v ) = -1; priority ( v ) = INT_MAX; //（在遍历树中的）父节点，优先级数
            for ( Rank u = 0; u < n; u++ ) //所有边的
                if ( exists ( v, u ) ) type ( v, u ) = UNDETERMINED; //类型
        }
    }
    void BFS ( Rank, int& ); //（连通域）广度优先搜索算法
    void DFS ( Rank, int& ); //（连通域）深度优先搜索算法
    bool TSort ( Rank, int&, Stack<Tv>* ); //（连通域）基于DFS的拓扑排序算法
    void BCC ( Rank, int&, Stack<Rank>& ); //（连通域）基于DFS的双连通分量分解算法
    template <typename PU> void PFS ( Rank, PU ); //（连通域）优先级搜索框架
public:
// 顶点
    int n; //顶点总数
    virtual Rank insert ( Tv const& ) = 0; //插入顶点，返回编号
    virtual Tv remove ( Rank ) = 0; //删除顶点及其关联边，返回该顶点信息
    virtual Tv& vertex ( Rank ) = 0; //顶点的数据（该顶点的确存在）
    virtual int inDegree ( Rank ) = 0; //顶点的入度（该顶点的确存在）
    virtual int outDegree ( Rank ) = 0; //顶点的出度（该顶点的确存在）
    virtual Rank firstNbr ( Rank ) = 0; //顶点的首个邻接顶点
    virtual Rank nextNbr ( Rank, Rank ) = 0; //顶点（相对当前邻居的）下一邻居
    virtual VStatus& status ( Rank ) = 0; //顶点的状态
    virtual int& dTime ( Rank ) = 0; //顶点的时间标签dTime
    virtual int& fTime ( Rank ) = 0; //顶点的时间标签fTime
    virtual Rank& parent ( Rank ) = 0; //顶点在遍历树中的父亲
    virtual int& priority ( Rank ) = 0; //顶点在遍历树中的优先级数
// 边：这里约定，无向边均统一转化为方向互逆的一对有向边，从而将无向图视作有向图的特例
    int e; //边总数
    virtual bool exists ( Rank, Rank ) = 0; //边(v, u)是否存在
    virtual void insert ( Te const&, int, Rank, Rank ) = 0; //在两个顶点之间插入指定权重的边
    virtual Te remove ( Rank, Rank ) = 0; //删除一对顶点之间的边，返回该边信息
    virtual EType & type ( Rank, Rank ) = 0; //边的类型
    virtual Te& edge ( Rank, Rank ) = 0; //边的数据（该边的确存在）
    virtual int& weight ( Rank, Rank ) = 0; //边(v, u)的权重
// 算法
    void bfs ( Rank ); //广度优先搜索算法
    void dfs ( Rank ); //深度优先搜索算法
    Stack<Tv>* tSort ( Rank ); //基于DFS的拓扑排序算法
    void bcc ( Rank ); //基于DFS的双连通分量分解算法
    template <typename PU> void pfs ( Rank, PU ); //优先级搜索框架
    void prim ( Rank ); //最小支撑树Prim算法
    void dijkstra ( Rank ); //最短路径Dijkstra算法
};


// Graph类相关函数实现
template<typename Tv, typename Te> //广度优先搜索BFS算法（单个连通域）
void Graph<Tv, Te>::BFS(Rank v, int & clock) { //assert: 0 <= v < n
    Queue<Rank> Q; //引入辅助队列
    status(v) = DISCOVERED; Q.enqueue(v); //起点入队
    while (!Q.empty()) { //在Q变空之前，不断
        Rank v = Q.dequeue(); dTime(v) == ++clock; //取出队首顶点v接受访问
        for (Rank u = firstNbr(v); -1 < u; u = nextNbr(v, u)) { //枚举v的所有邻居u
            if (UNDISCOVERED == status(u)) { //若u尚未被发现，则
                status(u) = DISCOVERED; Q.enqueue(u); //发现该顶点
                type(v, u) = TREE; parent(u) = v; //引入树边拓展支撑树
            } else { //若u已被发现、或者已经访问完毕，则
                type(v, u) = CROSS; //将(v, u)归类为跨边
            }
        }
        status(v) = VISITED; //至此，当前节点访问完毕
    }
}

template<typename Tv, typename Te> //广度优先搜索BFS算法（全图）
void Graph<Tv, Te>::bfs(Rank s) { // s < n
    reset(); int clock = 0; Rank v = s; //初始化
    do { //逐一检查所有顶点，发现所有连通域
        if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
            BFS(v, clock); //即从该顶点出发启动一次BFS
    } while (s != (v = ((v + 1) % n))); //按序号检查，故不漏不重
}

template<typename Tv, typename Te> //深度优先搜索DFS算法（单个连通域）
void Graph<Tv, Te>::DFS(Rank v, int & clock) { //v < n
    dTime(v) = ++clock; status(v) = DISCOVERED; //发现当前节点v
    for (Rank u = firstNbr(v); -1 < u; u = nextNbr(v, u)) { //枚举v的所有邻居u
        switch(status(u)) { //并视其状态分别处理
            case UNDISCOVERED: // //u尚未发现，意味着支撑树可在此拓展
                type(v, u) = TREE; parent(u) = v; DFS(u, clock); break;
            case DISCOVERED: //u已被发现但尚未访问完毕，应属被后代指向的祖先
                type(v, u) = BACKWARD; break;
            default: //u已访问完毕（VISITED，有向图），则视承袭关系分为前向边或跨边
                type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS; break;
        }
    }
    status(v) = VISITED;
    fTime(v) = ++clock; //至此，当前顶点v方告访问完毕
}

template<typename Tv, typename Te> //深度优先搜索DFS算法（全图）
void Graph<Tv, Te>::dfs(Rank s) { //s < n
    reset(); int clock = 0; Rank v = s; //初始化
    do { //逐一检查所有顶点
        if (UNDISCOVERED == status(v)) //一旦遇到尚未发现的顶点
            DFS(v, clock); //即从该顶点出发启动一次DFS
    } while (s != (v = ((v + 1) % n))); //按序号检查，故不漏不重
}

template<typename Tv, typename Te> //基于DFS的拓扑排序算法（单趟）
bool Graph<Tv, Te>::TSort(Rank v, int & clock, Stack<Tv> * S) { //v < n
    dTime(v) = ++clock; status(v) = DISCOVERED; // 发现顶点v
    for (Rank u = firstNbr(); -1 < u; u = nextNbr(v, u)) { //枚举v的所有邻居u
        switch (status(u)) { //并视u的状态分别处理
            case UNDISCOVERED:
                type(v, u) = TREE; parent(u) = v;
                if (!TSort(u, clock, S)) //从顶点u处出发深入搜索
                    return false; //若u及其后代不能拓扑排序（则全图亦必如此），故返回并报告
                break;
            case DISCOVERED:
                type(v, u) = BACKWARD; //一旦发现后向边（非DAG），则
                return false; //不必深入，故返回并报告
            default: ////VISITED (digraphs only)
                type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS;
                break;
        }
    }
    status(v) = VISITED; S->push(vertex(v)); //顶点被标记为VISITED时，随即入栈
    return true; //v及其后代可以拓扑排序
}

template<typename Tv, typename Te> //基于DFS的拓扑排序算法
Stack<Tv> *Graph<Tv, Te>::tSort(Rank s) { //assert: 0 <= s < n
    reset(); int clock = 0; Rank v = s;
    Stack<Tv>* S = new Stack<Tv>; //用栈记录排序顶点
    do {
        if (UNDISCOVERED == status(v)) //若已有连通域拓扑排序成功(S中已有元素)，此时进入新的连通域
            if (!TSort(v, clock, S)) { //新连通域存在环，
                while (!S->empty()) S->pop(); //则将原来S中的所有元素删除
                break; //退出外层循环
            }
    } while (s != (v = ((v + 1) % n)));
    return S; //若输入为DAG，则S内各顶点自顶向底排序；否则（不存在拓扑排序），S空
}

#define hca(x) (fTime(x)) //利用此处闲置的fTime[]充当hca[]
template<typename Tv, typename Te>
void Graph<Tv, Te>::BCC(Rank v, int & clock, Stack<Rank> & S) { //assert: 0 <= v < n
    hca(v) = dTime(v) = ++ clock; status(v) = DISCOVERED; S.push(v); //v被发现并入栈
    for (Rank u = firstNbr(v); -1 < u; u = nextNbr(v, u)) { //枚举v的所有邻居u
        switch ( (status(u))) { //并视u的状态分别处理
            case UNDISCOVERED:
                type(v, u) = TREE; parent(u) = v; BCC(u, clock, S); //从顶点u处深入
                if (hca(u) < dTime(v)) //遍历返回后，若发现u（通过后向边）可指向v的真祖先
                    hca(v) = min(hca(v), hca(u)); //则v亦必如此
                else { //否则，以v为关节点（u以下即是一个BCC，且其中顶点此时正集中于栈S的顶部）
                    while (v != S.pop()); //弹出当前BCC中（除v外）的所有节点，可视需要做进一步处理
                    S.push(v);
                }
                break;
            case DISCOVERED:
                type(v, u) = BACKWARD; //标记(v, u)，并按照“越小越高”的准则
                if (u != parent(v)) hca(v) = min(hca(v), dTime(u)); //更新hca[v]
                break;
            default: //VISITED (digraphs only)
                type(v, u) = (dTime(v) < dTime(u)) ? FORWARD : CROSS;
                break;
        }
    }
    status(v) = VISITED;
}
#undef hca

template<typename Tv, typename Te>
void Graph<Tv, Te>::bcc(Rank s) { //基于DFS的BCC分解算法
    reset(); int clock = 0; int v = s; Stack<int> S; //栈S用以记录已访问的顶点
    do {
        if (UNDISCOVERED == status(v)) { //一旦发现未发现的顶点（新连通分量）
            BCC(v, clock, S); //即从该顶点出发启动一次BCC
            S.pop(); //遍历返回后，弹出栈中最后一个顶点——当前连通域的起点
        }
    } while (s != (v = ((v + 1) % n)));
}

template <typename Tv, typename Te> template <typename PU> //顶点类型、边类型、优先级更新器
void Graph<Tv, Te>::PFS ( Rank v, PU prioUpdater ) { //优先级搜索（单个连通域）
    priority ( v ) = 0; status ( v ) = VISITED; parent ( v ) = -1; //初始化，起点v加至PFS树中
    while ( true ) { //将下一顶点和边加至PFS树中
        for ( Rank u = firstNbr ( v ); -1 < u; u = nextNbr ( v, u ) ) //对v的每一个邻居u
            prioUpdater ( this, v, u ); //更新其优先级及其父亲
        for ( int shortest = INT_MAX, u = 0; u < n; u++ )
            if ( UNDISCOVERED == status ( u ) ) //从尚未加入遍历树的顶点中
                if ( shortest > priority ( u ) ) //选出下一个
                { shortest = priority ( u ); v = u; } //优先级最高的顶点v
        if ( VISITED == status ( v ) ) break; //直至所有顶点均已加入
        status ( v ) = VISITED; type ( parent ( v ), v ) = TREE; //将v及与其父的联边加入遍历树
    }
} //通过定义具体的优先级更新策略prioUpdater，即可实现不同的算法功能

template <typename Tv, typename Te> template <typename PU> //优先级搜索（全图）
void Graph<Tv, Te>::pfs ( Rank s, PU prioUpdater ) { //s < n
    reset(); Rank v = s; //初始化
    do //逐一检查所有顶点
        if ( UNDISCOVERED == status ( v ) ) //一旦遇到尚未发现的顶点
            PFS ( v, prioUpdater ); //即从该顶点出发启动一次PFS
    while ( s != ( v = ( ( v+1 ) % n ) ) ); //按序号检查，故不漏不重
}

#endif //DSA_GRAPH_H
