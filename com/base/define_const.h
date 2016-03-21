#ifndef MATCHSERVER_MSPLUGIN_DEFINE_CONST_H
#define MATCHSERVER_MSPLUGIN_DEFINE_CONST_H

#include <stdlib.h>
#include <string.h>
#include <mscom/base/common.h>
#include <mscom/base/log.h>
#include <list>
#include <set>
#include <vector>
#include <string>
/////////////////////////////////////////////////////////////
/////////my define
#define MS_TO_FRONT_STATUS_GOOD "0"
#define MS_TO_FRONT_STATUS_ERROR "1"

////////////////////////////////////////////////////////////
///defined const in ms_plugin.h
#define MAX_ROW_COUNT (100)
#define MIN_ROW_COUNT (20)
#define BID_UNIT (10)
#define USER_LENGTH (256)
#define HTML_LENGTH (16384)
#define UID_MAX_LEN (64)
#define URL_MAX_LEN (1024)
//add macro follow zkfUNFOLLOWEDNO_FOLLOW 0


#ifndef TRACE_TIME
#define TRACE_TIME
#endif
#define COM_FLAG 0
#define GUESS_FLAG 1

#define FQ_UNKOWN -1
#define FQ_FOLLOWED 1
#define FQ_UNFOLLOWED 2
#define FQ_NOINTERESTED 3


#define S_VERSION "2.0"
#define VERSION "1.0"

////////////////////////////////////////////////////////////
/////original definedconst
////////////////////////////////////////////////////////////
#define SEP_A ''
#define SEP_B ''
#define SEP_C ''
#define SEP_D ''
#define SEP_E ''
#define SEP_F ''
#define MY_ADD "我关注的人中："
#define CLICK_ONE "对此感兴趣"
#define CLICK_TWO "人对此感兴趣"
#define FOLLOW_ONE "关注了"
#define FOLLOW_TWO "人关注了"
#define TOTAL_ONE "人对此感兴趣"
#define TOTAL_DONE "有"
#define DG  "等共"
#define PC "、"

#define GA_D  "等"
#define GA_FRI_FOLLOW "个间接关注"
#define FOLLOW_THREE "也关注了他"
#define GA_R "人"
#define BK "包括"
#define BUCKET_REP ":"
#define DEF_INTEREST "人气用户"

enum InfoFeild {
    IF_MODE = 0,
    IF_ID,
    IF_WEIGHT,
    IF_MAXPRICE,
    IF_PSID,
    IF_OPC,
    IF_VERSION,
    IF_END,
    IF_TOTAL
};

enum DataField {
    DF_ID = 0,
    DF_ADID,
    DF_ORDERNAME,
    DF_TITLE,
    DF_CUSTID,
    DF_ADTYPE,
    DF_BID,
    DF_IMPRESSION,
    DF_PRICE,
    DF_ADDESC,
    DF_CATID,
    DF_CREATIVES,
    DF_END,
    DF_TOTAL
};

enum CreativeField {
    CF_ID = 0,
    CF_URL,
    CF_TID,
    CF_WEIGHT,
    CF_HTML,
    CF_TOTAL
};

enum OpcField {
    OF_AGE = 0,
    OF_GENDER,
    OF_LOC,
    OF_TAG1,
    OF_TOTAL
};

enum RsStatus {
    RS_ERROR = -1,
    RS_NULL = 0,
    RS_CLICK = 1,
    RS_FOLLOW = 2,
    RS_TOTAL = 3
};

typedef struct CreativeInfo {
    std::string id;

    std::string url;

    std::string tid;

    float weight;

    std::string html;

    bool operator< (const CreativeInfo &c) const { //for sort
        return weight > c.weight;
    }
} CreativeInfo;

typedef struct OpcInfo {
    std::string age;

    std::string gender;

    std::set<std::string> loc;

    std::set<std::string> tag1;

} OpcInfo;

typedef struct DSAdInfo {
    std::vector<std::string> infovec;
    std::vector<std::string> advec;

    std::list<CreativeInfo> crs;
    std::set<std::string> psidset;
    OpcInfo adopc;
    int bidPrice;
    float chance;
    bool operator< (const DSAdInfo &a) const {
        return chance > a.chance;
    }

} DSAdInfo;

enum RetFrontField {
    RFF_ADID = 0,
    RFF_BUCKETID,
    RFF_MATCHMODE,
    RFF_ADTYPE,
    RFF_MAXPRICE,
    RFF_BIDPRICE,
    RFF_SCORE,
    RFF_LINKURL,
    RFF_UID,
    RFF_CATID,
    RFF_EXECODE,
    RFF_TOTAL
};

enum RetFoQuery {
    FO_OK = 0,
    FO_PACKAGE_ERROR,
    FO_SEND_ERROR,
    FO_RECV_ERROR,
    FO_PARSE_ERROR,
    FO_STATUS_ERROR,
    FO_SIZE_ERROR,
    FO_ZERO,
    FO_NEW_ERROR,
    FO_UID_ZERO
};
enum retError {
    ERR_NO = 0,
    ERR_QUERY,
    ERR_PLUGIN,
    ERR_PARSEQ,
    ERR_SEARCHINFO,
    ERR_SORT,
    ERR_DOBID,
    ERR_FILTER,
    ERR_SEARCHADDATA,
    ERR_TOSTRING,
    ERR_POINTER_NULL,
    ERR_FOLLOW_INIT = 50000,
    ERR_FO_PACKAGE_ERROR,
    ERR_FO_SEND_ERROR,
    ERR_FO_RECV_ERROR,
    ERR_FO_PARSE_ERROR,
    ERR_FO_STATUS_ERROR,
    ERR_FO_SIZE_ERROR,
    ERR_FO_ZERO,
    ERR_FO_NEW_ERROR,
    ERR_FO_UID_ZERO

};

/****************ms module begin******************/
enum ERR_CODE_SELECT {
    SELECT_OK = 0,
    SELECT_ERR = 300,
    SELECT_ERR_ATTACH_NET_OBJ_NULL ,
    SELECT_ERR_ATTACH_NET_ALREADY,
    SELECT_ERR_DETACH_NET_OBJ_NULL,
    SELECT_ERR_DETACH_NET_NOT_ATT,
    SELECT_ERR_NO_NET_CORE,
    SELECT_NO_CACHE,
    SELECT_ERR_INIT,
    SELECT_ERR_INIT_SPIO,
    SELECT_ERR_LOAD_DEF_AD,
    SELECT_ERR_NULL_POINTER,
    SELECT_ERR_AD_SIZE_ZERO,
    SELECT_ERR_MISS_PARAM,
    SELECT_ERR_SELECT_CREATIVE,
    SELECT_NO_DEF_AD,
    SELECT_ERR_GET_CREATIVE,
    SELECT_ERR_PROTO,
    SELECT_ERR_REQUEST_DS,
    SELECT_ERR_GET_NO_AD
};
enum ERR_CODE_FILTER {
    FILTER_RET_OK = 0,
    FILTER_RET_ERR = 400,
    FILTER_RET_ERR_NET_OBJ_IS_NULL ,
    FILTER_RET_ERR_NET_OBJ_IS_NOT_INIT,
    FILTER_RET_ERR_FO_OBJ_IS_NULL,
    FILTER_RET_ERR_FO_OBJ_IS_NOT_INIT,
    FILTER_RET_ERR_FO_OBJ_IS_ALREADY_ATT,
    FILTER_RET_ERR_INIT_FREQ,
    FILTER_RET_ERR_INIT_FO,
    FILTER_RET_ERR_FILTER_FREQ,
    FILTER_RET_ERR_FILTER_PSID,
    FILTER_RET_ERR_GUESS_PSID_NOT_EXIST,
    FILTER_RET_ERR_FREQ_NULL_PTR,
    FILTER_RET_ERR_EMPTY_AD_SIZE,
    FILTER_RET_ERR_PROTO_PARSE,
    FILTER_RET_ERR_FREQ_IS_NULL,
    FILTER_RET_ERR_FREQ_GET,
    FILTER_RET_ERR_FREQ_RETVALUE,
    FILTER_RET_ERR_GET_FO,
    FILTER_RET_ERR_OVER_MEM,
    FILTER_RET_ERR_AD_SIZE_ZERO,
    FILTER_RET_ERR_INC_FREQ,
    FILTER_RET_ERR_SIZE_NOT_MATCH
};
enum ERR_CODE_FREQ_CTL {
    FREQ_CTL_OK = 0,
    FREQ_CTL_ERR = 500,
    FREQ_CTL_ERR_ATTACH_NET_OBJ_NULL,
    FREQ_CTL_ERR_ATTACH_NET_ALREADY,
    FREQ_CTL_ERR_DETACH_NET_OBJ_NULL,
    FREQ_CTL_ERR_DETACH_NET_NOT_ATT,
    FREQ_CTL_ERR_INIT,
    FREQ_CTL_ERR_NO_NET_CORE,
};
enum ERR_CODE_NET_AGENT {
    NET_AGENT_OK = 0,
    NET_AGENT_ERR = 600,
    NET_AGENT_ERR_INIT_SPIO_CFG,
    NET_AGENT_ERR_INIT_JOIN_CLIENT,
    NET_AGENT_ERR_REQ_NOT_INIT,
    NET_AGENT_ERR_REQ_SEND,
    NET_AGENT_ERR_REQ_RECV
};

enum ERR_CODE_HANDLER {
    HANDLER_OK = 0,
    HANDLER_ERR = 700,
    HANDLER_INIT_MODULE_ERR,
    HANDLER_INIT_ERR,
    HANDLER_INVALID_PARAM,
    HANDLER_SEARCH_ERR,
    HANDLER_SELECT_DEF_AD_ERR,
    HANDLER_TARGET_ERR,
    HANDLER_RANK_ERR,
    HANDLER_SELECT_ERR

};
enum ERR_CODE_CONFIGURE {
    CONFIGURE_OK = 0,
    CONFIGURE_ERR = 800
};

const int FILTER_MASK_FREQ = 0x1;
const int FILTER_MASK_PSID = 0x2;
const int FILTER_MASK_ALL = 0x3;
const uint64_t PREFIX_ADID_4_FREQ = 0xFFFF0000;
const int NET_NOT_INIT = 0;
const int NET_IS_INIT = 1;
const int FREQ_DEF_THRESHOLD =  5;
const int HOURE = 60 * 60 * 60;
const int NET_AGENT_TMP_BUFF_SIZE = 1024;
const uint64_t SELECT_CLEAR_CACHE_TIMESPAN = 3600 * 24;
const uint64_t INVALID_PROTO_INT_VALUE = 0xFFFFFFFF;
const char INVALID_PROTO_STR_VALUE[] =  "invalid";
const int HOSTNAME_PREFIX_MAX = 64;
const char HANDLER_ALGO_PLUGIN_RANK_NAME[] = "rank";
const char HANDLER_ALGO_PLUGIN_TARGET_NAME[] =  "target";

/*****************configure*************/
const char CONFIG_FILED_FILTER[] = "filter";
const char CONFIG_FILED_COMMON[] = "common";
const char CONFIG_COMMON_DEFAULT_AD[] = "default_ad";
const char CONFIG_FILED_SELECT[] = "select_ds";
const char CONFIG_SELECT_NET_FILE_NAME[] = "select_ds_spio_file_name";
const char CONFIG_SELECT_NET_TIME_OUT[] = "select_ds_spio_timeout";
const char CONFIG_SELECT_NET_GROUP_NAME[] = "select_ds_spio_group_name";
const char CONFIG_SELECT_CREATIVE_CACHE_COUNT[] = "select_creative_cache_count";
const char CONFIG_SELECT_PSID_CIRCLE[] = "select_psid_circle";//psid:circle count
const char CONFIG_SELECT_PSID_CIRCLE_DEF[] = "select_psid_circle_def";//psid:circle count
const char CONFIG_FILED_FREQ[] = "frequence";
const char CONFIG_FREQ_NET_FILE_NAME[] = "frequence_spio_file_name";
const char CONFIG_FREQ_NET_TIME_OUT[] = "frequence_spio_timeout";
const char CONFIG_FREQ_NET_GROUP_NAME[] = "frequence_spio_group_name";
const char CONFIG_FREQ_FREQUENCE_THRESHOLD[] = "frequence_threshold";

const char CONFIG_FILED_PSID_CFG[] = "psid_cfg";
const char CONFIG_PSID_CFG_REQ_RELATION[] = "request_relation";
const char CONFIG_PSID_CFG_RELATION_PRIOR[] = "relation_prior";
const char CONFIG_PSID_CFG_GUESS_PSID[] = "guess_psid";

/****************ms module end******************/

#define ADTYPE_CPMWEB 2//cpm web 
#define ADTYPE_CPMWLS 2//cpm wireless
#define ADTYPE_CPD 6//cpd 
#define ADTYPE_BRAND 2//brand

/*
#define ADTYPE_CPMWEB_STR '2'//cpm web
#define ADTYPE_CPMWLS_STR '3'//cpm wireless
#define ADTYPE_CPD_STR '6'//cpd
*/


#endif //MATCHSERVER_DEFINE_CONST_H
