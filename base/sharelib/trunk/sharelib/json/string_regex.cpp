#include <string>
#include <inttypes.h>
#include <stdio.h>

#include "sharelib/json/string_tools.h"
#include "sharelib/json/exception.h"

SHARELIB_BS;

#define SUBSLEN 64

bool RegexMatch(const std::string& str, const std::string& pattern, const int& regex_mode)
{
    regex_t reg;
    int rtn;

    rtn = regcomp(&reg, pattern.c_str(), regex_mode);
    if (rtn != 0)
        SHARELIB_JSON_THROW(ParameterInvalidException, "Invalid regex pattern: " + pattern);
    rtn = regexec(&reg, str.c_str(), 0, 0, 0);
    regfree(&reg);
    switch(rtn)
    {
        case 0:
            return true;
        case REG_NOMATCH:
            return false;
        default:
            SHARELIB_JSON_THROW(ParameterInvalidException, "Error when matching regex: " + str + " with: " + pattern);

    }
};

bool RegexMatchExtension(const std::string& str, const std::string& pattern)
{
    return RegexMatch(str, pattern, REG_EXTENDED);
};

bool RegexGroupMatch(const std::string &originStr, const std::string& pattern, std::vector<std::string>& matched_items)
{
    bool IsFind;
    // size_t len;
    regex_t regex;
    int err;
    regmatch_t subs[SUBSLEN];

    err = regcomp(&regex, pattern.c_str(), REG_EXTENDED);
    if (err != 0)
    {
        regfree(&regex);
        SHARELIB_JSON_THROW(ParameterInvalidException, "Invalid regex pattern: " + pattern);
    }
    else
    {
        err = regexec(&regex, originStr.c_str(), (size_t)SUBSLEN, subs, 0);
    }

    switch (err)
    {
    case 0:
        IsFind = true;
        break;
    case REG_NOMATCH:
        IsFind = false;
        break;
    default:
        regfree(&regex);
        SHARELIB_JSON_THROW(ParameterInvalidException, "Error when matching regex: " + originStr + " with: " + pattern);
    }
    if (IsFind == false)
    {
        regfree(&regex);
        return false;
    }

    matched_items.clear();
    for (typeof(regex.re_nsub) i = 0; i <= regex.re_nsub; i++) 
    {
        int32_t length = subs[i].rm_eo - subs[i].rm_so;
        if (i == 0)
        {
            continue; /// the first match is the whole string
        }
        else
        {
            matched_items.push_back(std::string(originStr.c_str() + subs[i].rm_so, length ));
        }
    }
    regfree(&regex);
    return IsFind;
};

SHARELIB_ES;
