#include <iostream>
#include "smalltable2.hpp"
using st::eq;
using st::VAR;
using st::Maybe;

DEFINE_ATTRIBUTE(A, int);
DEFINE_ATTRIBUTE(B, int);
DEFINE_ATTRIBUTE(C, int);

typedef ST_TABLE(A, B, ST_UNIQUE_KEY(A), ST_UNIQUE_KEY(B, ST_CLUSTER_KEY(A))) T1;
typedef ST_TABLE(B, C, ST_UNIQUE_KEY(B)) T2;

int main () 
{
	T1 t1;  
	t1.init();
	t1.insert(1, 2);
	t1.insert(2, 2);
	t1.insert(3, 2);
	t1.insert(3, 3);
	t1.insert(4, 4);

	T2 t2;  
	t2.init(); 
	t2.insert(1, 10);
	t2.insert(2, 20);
	t2.insert(3, 31);
	t2.insert(4, 40);

	std::cout << "Content of t1:" <<  std::endl;
	for (T1::Iterator it = t1.begin(); it != t1.end(); ++it) { 
		std::cout << "A=" << it->at<A>() << ", B=" << it->at<B>() << std::endl;
	}

	std::cout << "Content of t2:" <<  std::endl;
	for (T2::Iterator it = t2.begin(); it != t2.end(); ++it) { 
		std::cout << "B=" << it->at<B>() << ", C=" << it->at<C>() << std::endl;
	}

        std::cout<<" select "<<std::endl;
        typedef ST_SELECTOR(ST_FROM(T1, Maybe<T2>), ST_WHERE(eq(TBL1<B>, TBL2<B>))) S;
        S s(&t1, &t2);
        for (S::Iterator it = s.select(); it; ++it) {
            std::cout << "A=" << it->at<TBL1, A>() << ", B=" << it->at<TBL1, B>() << ", C=" << it->at<TBL2, C>() << std::endl;
        }

	return 0;
}
