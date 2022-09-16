//
// Created by 王锐 on 2021/11/2.
//

#ifndef DSA_FIB_H
#define DSA_FIB_H

class Fib{  //Fibonacci数列类
private:
    int f, g;   //f = fib(k-1) g = fib(k)
public:
    Fib(int n){ // 初始为不小于n的最小Fibonacci项，创建时间O(log_phi(n=hi-lo))
        f = 1; g = 0;
        while (g < n) next();
    }
    int get() const {return g;}   // 获取当前Fibonacci项
    int next() {    //转至下一Fibonacci项
        g += f;
        f =  g - f;
        return g;
    }
    int prev(){     //转至上一Fibonacci项
        f = g - f;
        g -= f;
        return g;
    }
};

#endif //DSA_FIB_H
