#include <stdio.h>
using namespace std;


template <typename _Key, typename _Value>
class Dummy1 {
public:
    long run (_Key k, _Value v) __attribute__((noinline))
    {
        long s = 0;
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + static_cast<long>(v);
        }
        return s;
    }
};


template <typename _Key>
class __Dummy2 {
public:
    long __run (_Key k, const void* v) __attribute__((noinline))
    {
        long s = 0;
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        for  (int i=0; i<100; ++i) {
            s += static_cast<long>(k) + *static_cast<const long*>(v);
        }
        return s;
    }
};

template  <typename _Key, typename _Value>
class Dummy2 : __Dummy2<_Key>{
public:
    long run (_Key k, _Value v)
    {
        return this->__run(k, &v);
    }
};


int main ()
{
    printf("%ld\n", Dummy1<int, long>().run(1,1));
    printf("%ld\n", Dummy1<int, long>().run(1,1));
    printf("%ld\n", Dummy1<int, long>().run(1,1));
    printf("%ld\n", Dummy1<int, short>().run(1,1));
    printf("%ld\n", Dummy1<short, long>().run(1,1));
    printf("%ld\n", Dummy1<short, long>().run(1,1));
    printf("%ld\n", Dummy1<short, long>().run(1,1));
    printf("%ld\n", Dummy1<short, int>().run(1,1));
    printf("%ld\n", Dummy1<short, char>().run(1,1));

    printf("%ld\n", Dummy2<int, long>().run(1,1));
    printf("%ld\n", Dummy2<int, long>().run(1,1));
    printf("%ld\n", Dummy2<int, long>().run(1,1));
    printf("%ld\n", Dummy2<int, short>().run(1,1));
    printf("%ld\n", Dummy2<short, long>().run(1,1));
    printf("%ld\n", Dummy2<short, long>().run(1,1));
    printf("%ld\n", Dummy2<short, long>().run(1,1));
    printf("%ld\n", Dummy2<short, int>().run(1,1));
    printf("%ld\n", Dummy2<short, char>().run(1,1));
}
