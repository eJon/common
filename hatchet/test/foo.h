// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// @author	zhuxingchang@baidu.com
// @date	18/8/2010
// @brief	basic test class
#ifndef _FOO_H_
#define _FOO_H_

#include "string_writer.hpp"
using namespace st;

class foo
{
 private:
    int     a;
    int     b;
 public:
    void to_string(StringWriter &sb) const
    {
        sb << "(" << a << "," << b << ")";
    }

    friend std::ostream& operator<< (std::ostream& os, const foo& f)
    { return os << "{a=" << f.a << " b=" << f.b << "}"; }

    
    explicit foo( int a, int b)
    {
        this->a = a;
        this->b = b;
    }
    foo()
    {
        a=0;
        b=0;
    }
    bool operator <(const foo &other)const
    {
        if ( a < other.a )
            return true;
        else if ( a > other.a )
            return false;
        else
            if ( b < other.b )
                return true;
        return false;
    }
    bool operator >(const foo &other)const
    {
        if ( a > other.a )
            return true;
        else if ( a < other.a )
            return false;
        else
            if ( b > other.a )
                return true;
        return false;
    }
    bool operator ==(const foo &other) const
    {
        if ( a==other.a && b==other.b)
            return true;
        else
            return false;
    }
    bool operator !=(const foo &other) const
    {
        if ( a!=other.a || b!=other.b)
            return true;
        else
            return false;
    }

    
};

class foo2
{
 private:
 public:
 foo2():a(0),b(0),c(0),d(0){}
 foo2(int x, int y, int z, int k):a(x),b(y),c(z),d(k){}
    int get_A()
    {
        return a;
    }
    void set_A(int a)
    {
        this->a = a;
    }
    int get_B()
    {
        return b;
    }
    void set_B(int b)
    {
        this->b = b;
    }
    int get_C()
    {
        return c;
    }
    void set_C(int c)
    {
        this->c = c;
    }
    int get_D()
    {
        return d;
    }
    void set_D(int d){this->d = d;}

friend std::ostream& operator<< (std::ostream& os, const foo2& f)
    { return os << "{a=" << f.a << " b=" << f.b << " c=" << f.c << " d=" << f.d << "}"; }
      
 private:
    int a;
    int b:10;
    int c:20;
    int d:2;
};

class foo3
{
 public:
 foo3():id(0),literal(NULL)
    {}
    foo3(const int id, const char * literal)
    {
        this->id = id;
        if ( literal == NULL )
        {
            this->literal = (char*)malloc(1);
            this->literal[0]=0;
            return;
        }
        int len = strlen(literal);
        this->literal = (char*)malloc(sizeof(char)*(len+1));
        strncpy(this->literal, literal, len);
        this->literal[len]=0;
    }
    ~foo3()
    {
        if ( literal )
        {
            free(literal);
            literal=NULL;
        }
    }
    char * get() const
    {
        return literal;
    }
    foo3(const foo3 &rhs)
    {
        id = rhs.id;
        int len = strlen(rhs.literal);
        this->literal = (char*)malloc(sizeof(char)*(len+1));
        strncpy(this->literal, rhs.literal, len);
        this->literal[len]=0;
    }
    foo3 & operator=(const foo3 &other)
        {
            if ( this != &other )
            {
                size_t  len = strlen(other.literal);
                char * temp = (char*)malloc(len+1);
                strcpy(temp, other.literal);

                free(literal);
                this->literal = temp;
                id = other.id;
            }
            return *this;
        }
    bool operator < (const foo3 & rhs) const
    {
        return id < rhs.id;
    }
    void to_string(StringWriter &sb) const
    {
        sb << "(" << id << "," << literal << ")";
    }
 private:
    int   id;
    char* literal;
};

#endif  //_FOO_H_
