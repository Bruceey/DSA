//
// Created by 17634 on 2022/9/2.
//

#ifndef DSA_DICTIONARY_H
#define DSA_DICTIONARY_H

template<typename K, typename V>
struct Dictionary { //词典Dictionary模板类
    virtual int size() const = 0; //当前词条总数
    virtual bool put(K , V ) = 0; //插入词条（禁止雷同词条时可能失败）
    virtual V* get(K k) = 0; //读取词条
    virtual bool remove(K k) = 0; //删除词条
};

#endif //DSA_DICTIONARY_H
