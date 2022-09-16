//
// Created by 17634 on 2022/8/25.
//

#ifndef DSA_BINNODE_MACRO_BASIC_H
#define DSA_BINNODE_MACRO_BASIC_H

/******************************************************************************************
 * BinNode状态与性质的判断（参数均为对象）
 ******************************************************************************************/
#define IsRoot(x) ( ! ( (x).parent ) )
#define IsLChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->lc ) )
#define IsRChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->rc ) )
#define HasParent(x) ( ! IsRoot(x) )
#define HasLChild(x) ( (x).lc )
#define HasRChild(x) ( (x).rc )
#define HasChild(x) ( HasLChild(x) || HasRChild(x) ) //至少拥有一个孩子
#define HasBothChild(x) ( HasLChild(x) && HasRChild(x) ) //同时拥有两个孩子
#define IsLeaf(x) ( ! HasChild(x) )

/******************************************************************************************
 * 与BinNode具有特定关系的节点及指针
 ******************************************************************************************/
 //第一个宏使用时必须有父亲，且其兄弟可能为空(参数是指针)
 //第二个宏使用时必须有父亲，且其叔叔可能为空(参数是指针)
 //第三个宏结果多数是来自父亲的引用，但参数是根节点时结果为_root（参数是对象）
#define sibling( p ) ( IsLChild( * (p) ) ? (p)->parent->rc : (p)->parent->lc ) /*兄弟*/
#define uncle( x ) ( sibling( (x)->parent ) ) /*叔叔*/
#define FromParentTo( x ) /*来自父亲的引用*/ \
   ( IsRoot(x) ? (this->_root) : ( IsLChild(x) ? (x).parent->lc : (x).parent->rc ) )

#endif //DSA_BINNODE_MACRO_BASIC_H
