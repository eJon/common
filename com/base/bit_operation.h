#ifndef BIDPLUGIN_INCLUDE_IDXSERVER_OPERATION_H_
#define BIDPLUGIN_INCLUDE_IDXSERVER_OPERATION_H_

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <vector>
#include <string>

inline uint32_t BitSize2ByteSize(uint32_t bit_len) {
    // Actually, the more accurate style is byte_len = (bit_len + 7) >> 3;
    // we use the follow style to ease the modification work.
    return (bit_len + 7) >> 3;
}

inline uint32_t ByteSizeAlignUInt64Size(uint32_t byte_len) {
    return ((byte_len + 7) >> 3) << 3;
}

inline uint32_t BitSizeAlignUInt64Size(uint32_t bit_len) {
    uint32_t size = BitSize2ByteSize(bit_len);
    return ByteSizeAlignUInt64Size(size);
}


inline uint32_t ByteSize2UInt64Size(uint32_t byte_len) {
    return (byte_len + 7) >> 3;
}

inline uint32_t ByteSize2UInt32Size(uint32_t byte_len) {
    return (byte_len + 3) >> 2 ;
}

inline bool IsUInt32(uint32_t size) {
    return size % 4 ? false : true;
}
inline bool IsUInt64(uint32_t size) {
    return size % 8 ? false : true;
}

inline uint32_t BitSize2UInt64Size(uint32_t bit_len) {
    return (bit_len >> 6) + 1;
}

enum OperationType {
    OR_OPERATION = 1,
    AND_OPERATION = 2
};

template<typename T>
inline int MemOperation(int oper_type,
                        const T *ptr, uint32_t size, T *ret_ptr) {
    uint32_t i = 0;

    for(i = 0; i < size; ++i) {
        if(AND_OPERATION == oper_type) {
            ret_ptr[i] &= ptr[i];
        } else if(OR_OPERATION == oper_type) {
            ret_ptr[i] |= ptr[i];
        }
    }

    return 0;
}
// param : type   : or, and ...
//         ch_ptr1: input uchar array 1,put result in ret_ptr, The memory is allocated outside
//         ch_ptr2: input uchar array 2
//         size  : ch_ptr2's length
//return : OK or Error
inline int BitOperation(int oper_type,
                        uint8_t *ch_ptr1, uint8_t *ch_ptr2,
                        uint32_t size) {
    if((NULL == ch_ptr1) || (NULL == ch_ptr2)) {
        return -1;
    }

    uint32_t type_size = 0;
    uint32_t i = 0;

    if(IsUInt64(size)) {
        type_size = ByteSize2UInt64Size(size);
        uint64_t *uint64_prt1 = (uint64_t *)ch_ptr1;
        uint64_t *uint64_prt2 = (uint64_t *)ch_ptr2;
        return MemOperation(oper_type, uint64_prt2, type_size, uint64_prt1);
    }

    if(IsUInt32(size)) {
        type_size = ByteSize2UInt32Size(size);
        uint32_t *uint32_prt1 = (uint32_t *)ch_ptr1;
        uint32_t *uint32_prt2 = (uint32_t *)ch_ptr2;
        return MemOperation(oper_type, uint32_prt2, type_size, uint32_prt1);
    } else {
        for(i = 0; i < size; ++i) {
            if(AND_OPERATION == oper_type) {
                ch_ptr1[i] = ch_ptr1[i] & ch_ptr2[i];
            } else if(OR_OPERATION == oper_type) {
                ch_ptr1[i] = ch_ptr1[i] | ch_ptr2[i];
            } else {
                return -1;
            }
        }
    }

    return 0;
}

template<typename T>
inline int ArrayMemOps(const std::vector<T *> &mem_array,
                       int ops_type_ex, int ops_type_intra,
                       uint32_t size, T *ret_map) {
    size_t i = 0, j = 0;
    T tmp;

    for(i = 0; i < size; ++i) {
        tmp = mem_array[0][i];

        for(j = 0; j < mem_array.size(); j++) {
            if(AND_OPERATION == ops_type_intra) {
                tmp &= mem_array[j][i];
            }

            if(OR_OPERATION == ops_type_intra) {
                tmp |= mem_array[j][i];
            }
        }

        if(AND_OPERATION == ops_type_ex) {
            ret_map[i] &= tmp;
        } else if(OR_OPERATION == ops_type_ex) {
            ret_map[i] |= tmp;
        }
    }

    return 0;
}

inline int ArrayBitOps(std::vector<uint8_t *> byte_array,
                       int ops_type_ex, int ops_type_intra,
                       uint32_t size, uint8_t *ret_map) {
    uint32_t type_size = 0;
    size_t i = 0;
    size_t vec_size = byte_array.size();

    if(IsUInt64(size)) {
        type_size = ByteSize2UInt64Size(size);
        std::vector<uint64_t *> uint64_vec(vec_size);

        for(i = 0; i < vec_size; ++i) {
            uint64_vec[i] = (uint64_t *)byte_array[i];
        }

        return ArrayMemOps(uint64_vec, ops_type_ex, ops_type_intra,
                           type_size, (uint64_t *)ret_map);
    }

    if(IsUInt32(size)) {
        type_size = ByteSize2UInt32Size(size);
        std::vector<uint32_t *> uint32_vec(vec_size);

        for(i = 0; i < vec_size; ++i) {
            uint32_vec.push_back((uint32_t *)byte_array[i]);
        }

        return ArrayMemOps(uint32_vec, ops_type_ex, ops_type_intra,
                           type_size, (uint32_t *)ret_map);
    }

    return ArrayMemOps(byte_array, ops_type_ex, ops_type_intra, size, ret_map);
}

inline int GetEffectiveBitIndex(std::vector<unsigned int> &vTargets,
                                unsigned char *source, size_t size) {
    static const unsigned char testtab[8] = {0x80, 0x40, 0x20, 0x10,
                                             0x08, 0x04, 0x02, 0x01
                                            };
    unsigned char thechar;
    size_t tmp_size = BitSize2ByteSize(size);

    for(unsigned int i = 0; i < tmp_size; i++) {
        thechar = *(source + i);

        if(thechar == 0) {
            continue;
        }

        for(unsigned int j = 0; j < 8; j++) {
            if((testtab[j] & thechar) * 1 > 0) {
                vTargets.push_back(i * 8 + j);
            }
        }
    }

    return 0;
}

inline int SetBit(unsigned char *pchar, const int &size,
                  unsigned int bit_index, int value) {
    uint32_t offset = bit_index >> 3;
    uint32_t offset_in_byte = bit_index & 0x7;

    if(pchar == NULL || offset > static_cast<uint32_t>(size - 1)) {
        return -1;
    }

    if(0 == value) {
        *(pchar + offset) &= ~(0x80 >> offset_in_byte);
    } else {
        *(pchar + offset) |= (0x80 >> offset_in_byte);
    }

    return 0;
}

inline int GetBit(unsigned char *pchar, const int &size,
                  unsigned int bit_index, uint32_t &value) {
    if(pchar == NULL || bit_index >= static_cast<unsigned int>(size << 3)) {
        return -1;
    }

    uint8_t *pstart = pchar;
    int ch_site = bit_index / 8;
    int bit_size = bit_index % 8;
    int tmp_index = 7 - bit_size;
    pstart += ch_site;
    uint8_t ch = *pstart & (1 << tmp_index);
    value = ((0 != ch) ? 1 : 0);
    return 0;
}

inline int CreateThread(void * (*thread_fun)(void *), void *arg) {
    pthread_t tid;
    pthread_attr_t opt;
    pthread_attr_init(&opt);
    pthread_attr_setdetachstate(&opt, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&tid, &opt, thread_fun, arg);
    pthread_attr_destroy(&opt);
    return ret;
}

// Get system current time
// Get effectvie bit size sum. If bit = 1, sum ++
inline int GetEffectiveNum(uint8_t *&bim_map, const int &size) {
    static unsigned int count_table[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
    };
    int i = 0, sum = 0;

    for(i = 0; i < size; ++i) {
        sum += count_table[bim_map[i]];
    }

    return sum;
}

#endif  // BIDPLUGIN_INCLUDE_IDXSERVER_OPERATION_H_
