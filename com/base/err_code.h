#ifndef __ERROR_CODE_DEF_H__
#define __ERROR_CODE_DEF_H__

#include <string>

#define RET_OK 0
#define RET_ERROR 1
const static std::string g_strParaErrMsg[] = {
    "3001:Parameter error: invalid parameter format",
    "3002:Parameter error: invalid uid",
    "3003:Parameter error: invalid timestamp",
    "3004:Parameter error: invalid appkey",
    "3005:Parameter error: invalid feed type",
    "3006:Parameter error: invalid psid"
};

const static std::string g_strFilterErrMsg[] = {
    "3101:ad filtered by white app list",
    "3102:ad filtered by timespan",
    "3103:ad filtered by total frequence",
    "3104:ad filtered by black user visited ",
    "3105:ad filtered by frequence access failed",
    "3106:ad filtered by frequence",
    "3107:ad filtered by shield access failed",
    "3108:ad filtered by already shield",
    "3109:ad filtered by unsupported client"
};

const static std::string g_strTargetErrMsg[] = {
    "3401:Target plugin select abnormal",
    "3402:Target plugin no exist",
    "3403:Target plugin run failed",
    "3404:Target plugin return null"

};
const static std::string g_strRankErrMsg[] = {
    "3501:Rank plugin select abnormal",
    "3502:Rank plugin no exist",
    "3503:Rank plugin run failed",
    "3504:Rank plugin return null"
};
const static std::string g_null = "";

#endif
