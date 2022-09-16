//
// Created by 17634 on 2022/9/4.
//

#ifndef DSA_BITMAP_H
#define DSA_BITMAP_H

#include <cstdlib>
#include <cstdio>
#include <memory.h>
#include "../_share/release.h"

class Bitmap { //位图Bitmap类
private:
    unsigned char* M; int N; //比特图所存放的空间M[]，容量为N*sizeof(char)*8比特
protected:
    void init(int n) { M = new unsigned char[N = (n + 7) / 8]; memset(M, 0, N); }
public:
    Bitmap(int n = 8) { init(n); } //按指定或默认规模创建比特图（为测试暂时选用较小的默认值）
//    Bitmap(char* file, )
};

#endif //DSA_BITMAP_H
