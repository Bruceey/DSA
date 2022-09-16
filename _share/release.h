//
// Created by 17634 on 2022/8/26.
//

#ifndef DSA_RELEASE_H
#define DSA_RELEASE_H

template <typename T> struct Cleaner {
    static void clean ( T x ) { //相当于递归基
#ifdef _DEBUG
        static int n = 0;
      if ( 7 > strlen ( typeid ( T ).name() ) ) { //复杂类型一概忽略，只输出基本类型
         printf ( "\t<%s>[%d]=", typeid ( T ).name(), ++n );
         print ( x );
         printf ( " purged\n" );
      }
#endif
    }
};

template <typename T> struct Cleaner<T*> {
    static void clean ( T* x ) {
        if ( x ) { delete x; } //如果其中包含指针，递归释放
#ifdef _DEBUG
        static int n = 0;
      printf ( "\t<%s>[%d] released\n", typeid ( T* ).name(), ++n );
#endif
    }
};

template <typename T> void release ( T x ) { Cleaner<T>::clean ( x ); }

#endif //DSA_RELEASE_H
