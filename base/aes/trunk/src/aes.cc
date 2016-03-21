#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <string.h>
#include "aes.h"
#include "_aes.h"

aes_t * 
aes_create(const u_char *key, int key_len)
{
    if (NULL == key || 0 >= key_len){
        return NULL;
    }
    aes_t *aes;
    aes = (aes_t *)malloc(sizeof(aes_t));
    memset(aes, 0, sizeof(aes_t));
    if (0 != private_AES_set_encrypt_key(key, key_len * 8, &aes->aes_key)){
        free(aes);
        return aes;
    }
    if (0 != private_AES_set_decrypt_key(key, key_len * 8, &aes->aes_dekey)){
        free(aes);
        return aes;
    }
    aes->key_len = key_len;
    aes->key = malloc(aes->key_len);
    memcpy(aes->key, key, aes->key_len);
    return aes;
}

aes_t *
aes_create(const char *file_name)
{
    if (NULL == file_name){
        return NULL;
    }
#ifdef WIN32
    int fd = _open(file_name, _O_BINARY|_O_RDONLY);
#else
    int fd = open(file_name, S_IRUSR);
#endif
    if (-1 == fd){
        return NULL;
    }
    unsigned char key[1024] = {};
    int r_index = read(fd, &key[0], sizeof(key));
    if (-1 == r_index || r_index == sizeof(key)){
        return NULL;
    }
    return aes_create(key, r_index);
}


int 
aes_destroy( aes_t *aes )
{
    if (NULL != aes){
        if (NULL != aes->key){
            free(aes->key);
            aes->key = NULL;
        }
        free(aes);
    }
    return 0;
}

static u_char c_map[] = {'0', '1', '2', '3',
                         '4', '5', '6', '7',
                         '8', '9', 'A', 'B',
                         'C', 'D', 'E', 'F'
                        };

static char c_rev_map[] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //0
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //16
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //32
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,  //48
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //64
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //80
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //96
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //112
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //128
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //144
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //160
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //176
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //192
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //208
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  //224
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   //240    
};

int 
aes_enc( aes_t *aes, u_char *src, uint32_t src_len, u_char *enc, uint32_t *enc_len )
{
    if (NULL == aes || NULL == src || NULL == enc || NULL == enc_len 
            || 0 == src_len || 0 != src[src_len -1 ]){
        return -1;
    }

    uint32_t ret_enc_len = (((src_len + (1 << 4) - 1) >> 4) << 4);
    if (*enc_len < ret_enc_len * 2){
        return -2;
    }
    for (uint32_t i = 0; i <= (src_len - 1 )>> 4; ++i){
        AES_encrypt(src + (i << 4), enc + (i << 4), &aes->aes_key);
    }
    for (int i = ret_enc_len - 1; i >= 0; i--){
        enc[2*i + 1] = c_map[enc[i] & 0xf];
        enc[2*i] = c_map[(enc[i] & 0xf0) >> 4] ;
    }
    *enc_len = ret_enc_len * 2;
    return 0;
}

int 
aes_dec( aes_t *aes, u_char *enc, uint32_t enc_len, u_char *src, uint32_t *src_len )
{
    if (NULL == aes || NULL == enc || NULL == src || NULL == src_len){
        return -1;
    }
    if (0 == enc_len || 0 != (enc_len & 0xf) || *src_len < (enc_len >> 1)|| 0 != (enc_len & 0x1)){
        return -2;
    }
    for (int i = 0; i <= enc_len - 1; i += 2){
        if(-1 == c_rev_map[enc[i]]){
            return -3;
        }
        enc[i >> 1] = (c_rev_map[enc[i]] << 4) + c_rev_map[enc[i + 1]];
    }
    for (uint32_t i = 0; i <= (enc_len - 1)>> 4; ++i){
        AES_decrypt(enc + (i << 4), src + (i << 4), &aes->aes_dekey);
    }
	enc_len >>= 1;
    if (0 == src[enc_len - 1]){
        *src_len = strlen((const char *)src) + 1;
    }else{
        *src_len = enc_len;
    }
    return 0;
}
