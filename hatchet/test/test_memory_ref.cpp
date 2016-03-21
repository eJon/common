#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct Node {
    int k;
};

struct A {
    char* p;
    Node n;
};

struct B {
    char* p;
    char buf[];
};

int main(int argc, char** argv)
{
    char choice = 'A';
    if (argc == 2) {
        choice = toupper(argv[1][0]);
    }
    
    A* a = (A*)malloc(sizeof(A));
    B* b = (B*)malloc(sizeof(B) + sizeof(Node));
    
    if (choice == 'A') {
        for (int i = -9999999; i < 999999999; i++) {
            a->p += 2;
            a->n.k += 2;
        }
        printf("%p\n", a->p);
        printf("%d\n", a->n.k);
        printf("sizeof(A)=%lu\n", sizeof(A));
    } else if (choice == 'B') {
        for (int i = -9999999; i < 999999999; i++) {
            b->p += 2;
            ((Node*)(b->buf))->k += 2;
        }
        printf("%p\n", b->p);
        printf("%d\n", ((Node*)(b->buf))->k);
        printf("sizeof(B)=%lu\n", sizeof(B) + sizeof(Node));
    } else {
        printf("Unknown choice=%c\n", choice);
    }
    
    free(a);
    free(b);
    
    return 0;
}
