#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "aes.h"

int
main(int argc, char **argv)
{
#ifdef WIN32
    aes_t *aes = aes_create(argv[1]);
#else
    aes_t *aes = aes_create("./aes.key");
#endif
    if (NULL == aes){
        return -1;
    }

    while (true){
        unsigned char src[1024] = {};
        int random_key = rand() % 1024;
        for (int i = 0; i < random_key; i++){
            src[i] = rand() % 128;
        }
        
        unsigned char enc_src[102400] = {};
        uint32_t enc_src_len = sizeof(enc_src);
        unsigned char dec_src[2048] = {};
        uint32_t dec_src_len = sizeof(dec_src);

        aes_enc(aes, src, random_key + 1, enc_src, &enc_src_len);
        aes_dec(aes, enc_src, enc_src_len, dec_src, &dec_src_len);
  
        if (0 != memcmp(src, dec_src, random_key)){
            printf("error\n");
        }
    }
    return 0;
}
