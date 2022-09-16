//
// Created by 王锐 on 2021/11/8.
//

#ifndef DSA_GRAPHMATRIX_H
#define DSA_GRAPHMATRIX_H

#include "Graph.h"   //引入图ADT

template<typename Tv>
struct Vertex { //顶点对象
    Tv data; int inDegree, outDegree; VStatus status; //数据出度、入度、状态
    int dTime, fTime; //时间标签: 被发现时刻、访问完毕时刻
    Rank parent; int priority; //在遍历树中的父节点、优先级数
    Vertex(Tv const& d = (Tv) 0): //构造新顶点
        data(d), inDegree(0), outDegree(0), status(UNDISCOVERED),
        dTime(-1), fTime(-1), parent(-1), priority(INT_MAX) {} //暂不考虑权重溢出
};

template<typename Te>
struct Edge {  //边对象
    Te data; int weight; EType type; //数据、权重、类型
    Edge(Te const& d, int w): data(d), weight(w), type(UNDETERMINED) {}
};

template<typename Tv, typename Te>
class GraphMatrix: public Graph<Tv, Te> { //基于向量，以邻接矩阵形式实现的图
private:
    Vector< Vertex<Tv> > V; //顶点集（向量）
    Vector< Vector<Edge<Te>*> > E;  //边集（邻接矩阵）
public:
    GraphMatrix() {this->e = this->n = 0;} //构造
    ~GraphMatrix() { //析构
        for ( Rank v = 0; v < this->n; v++ ) //所有动态创建的
            for ( Rank u = 0; u < this->n; u++ ) //边记录
                delete E[v][u]; //逐条清除
    }
// 顶点的基本操作：查询第v个顶点（0 <= v < n）
    virtual Tv& vertex ( Rank v ) { return V[v].data; } //数据
    virtual int inDegree ( Rank v ) { return V[v].inDegree; } //入度
    virtual int outDegree ( Rank v ) { return V[v].outDegree; } //出度
    virtual Rank firstNbr ( Rank v ) { return nextNbr ( v, this->n ); } //首个邻接顶点
    virtual Rank nextNbr ( Rank v, Rank u ) //相对于顶点u的下一邻接顶点（改用邻接表可提高效率）
    { while ( ( -1 < u ) && ( !exists ( v, --u ) ) ); return u; } //逆向线性试探
    virtual VStatus& status ( Rank v ) { return V[v].status; } //状态
    virtual int& dTime ( Rank v ) { return V[v].dTime; } //时间标签dTime
    virtual int& fTime ( Rank v ) { return V[v].fTime; } //时间标签fTime
    virtual Rank& parent ( Rank v ) { return V[v].parent; } //在遍历树中的父亲
    virtual int& priority ( Rank v ) { return V[v].priority; } //在遍历树中的优先级数
// 顶点的动态操作
    virtual Rank insert(Tv const& vertex) {  //插入顶点、返回编号; 这里仅仅插入顶点
        for (Rank u = 0; u < this->n; u++) E[u].insert(nullptr); //各顶点预留一条潜在的关联边(即原先矩阵的最后一列插入null)
        this->n++; //然后使得当前顶点总数+1
        E.insert(Vector<Edge<Te>*> (this->n, this->n, (Edge<Te>*) nullptr)); //创建新顶点对应的边向量
        return V.insert(Vertex<Tv> (vertex));  //顶点向量增加一个顶点
    }
    virtual Tv remove(Rank v) {  //删除第v个顶点及其关联边（0 <= v < n）
        for (Rank u = 0; u < this->n; u++)  //所有出边
            if (exists(v, u)) { delete E[v][u]; V[u].inDegree--; this->e--; }  //逐条删除
        E.remove(v); //删除边集第v行
        this->n--; //顶点数-1
        Tv vBak = vertex(v); V.remove(v); //预保存顶点v的data，删除顶点v
        for (Rank u = 0; u < this->n; u++)  //所有v的入边
            if ( Edge<Te>* x = E[u].remove(v)) { delete x; V[u].outDegree--;this->e--; }  //逐条删除
        return vBak; //返回被删除顶点的信息
    }
//边的确认操作
    virtual bool exists(Rank v, Rank u)  //边(v, u)是否存在
    { return (0 <= v) && (v < this->n) && (0 <= u) && (u < this->n) && (E[v][u] != nullptr); }
// 边的基本操作：查询顶点v与u之间的联边（0 <= v, u < n且exists(v, u)）
    virtual EType & type ( Rank v, Rank u ) { return E[v][u]->type; } //边(v, u)的类型
    virtual Te& edge ( Rank v, Rank u ) { return E[v][u]->data; } //边(v, u)的数据
    virtual int& weight ( Rank v, Rank u ) { return E[v][u]->weight; } //边(v, u)的权重
// 边的动态操作
    virtual void insert ( Te const& edge, int w, Rank v, Rank u ) { //插入权重为w的边e = (v, u)；参数edge表示边的data
        if ( exists ( v, u ) ) return; //确保该边尚不存在
        E[v][u] = new Edge<Te> ( edge, w ); //创建新边
        this->e++; V[v].outDegree++; V[u].inDegree++; //更新边计数与关联顶点的度数
    }
    virtual Te remove ( Rank v, Rank u ) { //删除顶点v和u之间的联边（exists(v, u)）
        Te eBak = edge ( v, u ); delete E[v][u]; E[v][u] = nullptr; //备份后删除边记录
        this->e--; V[v].outDegree--; V[u].inDegree--; //更新边计数与关联顶点的度数
        return eBak; //返回被删除边的信息
    }
};


#endif //DSA_GRAPHMATRIX_H
