#ifndef FREQCTRL_CONST_HEADER
#define FREQCTRL_CONST_HEADER

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum FREQCTRL_OPTYPE {
    FC_GET = 1,
    FC_SET,
    FC_INCR,
    FC_DECR,
    FC_ADD,
    FC_SUB,
    FC_RESET,
    FC_CLEAR,
    FC_EXPIRE,
    FC_DUMP,
    FC_RELOAD,
};

enum FREQCTRL_DATATYPE {
    FC_ADVISIT = 0x1,

    FC_FANS_QUERY_USER_PERM = 0x11,
    FC_FANS_INCR_ALL,
    FC_FANS_CLEAR_ALL,
    FC_FANS_RESET_USER,
    FC_FANS_QUERY_FEED_CYCLE,
    FC_FANS_CLEAR_COUNT,

    FC_AIM_QUERY_CUSTVISIT_LIMIT = 0x21,
    FC_AIM_INCR_CUSTVISIT_TODAY,
    FC_AIM_QUERY_VISIT_TOTAL,
    FC_AIM_CLEAR_VISIT_TOTAL,

    FC_TREND_QUERY_USER_PERM = 0x30,
    FC_TREND_QUERY_CUST_PERM,
    FC_TREND_QUERY_CUSTLIMIT,
    FC_TREND_QUERY_CUSTVISIT,
    FC_TREND_QUERY_ADVISIT,
    FC_TREND_QUERY_APPVISIT,
    FC_TREND_INCR_ALL,
    FC_TREND_CLEAR_ALL,

    FC_TAOBAO_QUERY_ALL = 0x40,
    FC_TAOBAO_QUERY_SITE,
    FC_TAOBAO_QUERY_USER,
    FC_TAOBAO_INCR_ALL,
    FC_TAOBAO_CLEAR_ALL,


    FC_BRAND = 0x50,
};

inline u32 GetCMD(u32 op_type, u32 data_type) {
    return ((op_type & 0xFF) << 24) + (data_type & 0xFFFFFF);
}

inline u32 GetOpType(u32 op) {
    return op >> 24;
}

inline u32 GetDataType(u32 op) {
    return op & 0xFFFFFF;
}

#endif
