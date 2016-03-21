#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "../st_timer.h"


struct base_t
{
    enum subtype { DERIVED1, DERIVED2, DERIVED3 };
    subtype type_;
    virtual void foo (int* x) = 0;
    //void foo (int* x) {}
};

struct derived1_t : public base_t
{
    derived1_t()
    {
        this->type_ = DERIVED1;
    }
    int x;
    
    inline void foo (int* o)
    {
        if (x < 100)
        {
            *o = x*x + x*2 + 1;
        }
        else
        {
            *o = x*x;
        }
    }
};

struct derived2_t : public base_t
{
    derived2_t()
    {
        this->type_ = DERIVED2;
    }
    int x;
    inline void foo (int* o)
    {
        if (x < 100)
            *o = x*x*x + 3*x*x + 3*x + 1;
        else
            *o = x*x*x;
    }
};

struct derived3_t : public base_t
{
    derived3_t()
    {
        this->type_ = DERIVED3;
    }
    int x;
    inline void foo (int* o)
    {
         *o = x*x*x + 3*x*x + 3*x + 1;
    }
};

// --------------------

struct adt_base_t
{
    enum subtype { DERIVED1, DERIVED2, DERIVED3 };
    subtype type_;
    int x;
    
    inline void foo (int* o) __attribute__((always_inline))
    {
        switch (type_)
        {
            case DERIVED1:
                {
                    if (x < 100)
                        *o = x*x + x*2+1;
                    else
                        *o = x*x;
                }
                break;
            case DERIVED2:
                {
                    if (x < 100)
                        *o = x*x*x + 3*x*x + 3*x + 1;
                    else
                        *o = x*x*x;
                }
                break;
            case DERIVED3:
                *o = x*x*x + 3*x*x + 3*x + 1;
                break;
        }
    }
};

struct adt_derived1_t : public adt_base_t
{
    adt_derived1_t()
    {
        this->type_ = DERIVED1;
    }
};

struct adt_derived2_t : public adt_base_t
{
    adt_derived2_t()
    {
        this->type_ = DERIVED2;
    }
};


struct adt_derived3_t : public adt_base_t
{
    adt_derived3_t()
    {
        this->type_ = DERIVED3;
    }
};

int main ()
{
    srand (time(0));
    printf ("hello world!\n");
    const int REGR = 1000000;
    base_t** a = new base_t*[REGR];
    adt_base_t** b = new adt_base_t*[REGR];
    derived2_t** c = new derived2_t*[REGR];
    int a_size=0, b_size=0, c_size=0;
    //std::vector<base_t*> a;
    //std::vector<adt_base_t*> b;
    for (int i=0; i<REGR; ++i)
    {   
        /*
        int r = rand()% 100;
        if (r < 30)
        {
            //a.push_back (new derived1_t());
            //b.push_back (new adt_derived1_t());
            a[a_size++] = new derived1_t();
            b[b_size++] = new adt_derived1_t();
        }
        else if (r < 60)
        {*/
            //a.push_back (new derived2_t());
            //b.push_back (new adt_derived2_t());
            a[a_size++] = new derived2_t();
            b[b_size++] = new adt_derived2_t();
            c[c_size++] = new derived2_t();
    /*    
    }
        else
        {
            //a.push_back (new derived3_t());
            //b.push_back (new adt_derived3_t());
            a[a_size++] = new derived3_t();
            b[b_size++] = new adt_derived3_t();
        }
        */
    }

    NaiveTimer t;
    
    t.start();
    int r=0;
    int v;
    for (size_t i=0,i_e=/*a.size()*/a_size; i<i_e; ++i)
    {
        a[i]->foo(&v);
        r += v+i;
    }
    t.stop();
    printf ("a c=%d, time=%ld\n", r, t.u_elapsed());

    t.start();
    r=0;
    for (size_t i=0,i_e=/*a.size()*/b_size; i<i_e; ++i)
    {
        b[i]->foo(&v);
        r+= v+i;
    }
    t.stop();
    printf ("b c=%d, time=%ld\n", r, t.u_elapsed());
    
    t.start();
    r=0;
    for (size_t i=0,i_e=/*a.size()*/c_size; i<i_e; ++i)
    {
        c[i]->foo(&v);
        r+=v+i;
    }
    t.stop();
    printf ("c c=%d, time=%ld\n", r, t.u_elapsed());

    
    t.start();
    r=0;
    for (size_t i=0,i_e=/*a.size()*/c_size; i<i_e; ++i) {
        int x = 0;
        int *o = &v;
        switch (b[i]->type_) {
            case adt_base_t::DERIVED1:
                {
                    if (x < 100)
                        *o = x*x + x*2+1;
                    else
                        *o = x*x;
                }
                break;
            case adt_base_t::DERIVED2:
                {
                    if (x < 100)
                        *o = x*x*x + 3*x*x + 3*x + 1;
                    else
                        *o = x*x*x;
                }
                break;
            case adt_base_t::DERIVED3:
                *o = x*x*x + 3*x*x + 3*x + 1;
                break;
        }
        r+=v+i;
    }
    t.stop();
    printf ("c c=%d, time=%ld\n", r, t.u_elapsed());

}
