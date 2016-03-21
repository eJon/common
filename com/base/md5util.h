#ifndef _MD5UTIL_H__
#define _MD5UTIL_H__
#include "md5.h"

class Md5Util {
  public:
    static string Encode(const string uid) {
        static string key = "sina";
        string ret;
        string new_uid;
        char ch;
        bool invalid = false;

        for(size_t i = 0; i < uid.length(); ++i) {
            ch = uid[i];

            if(ch < '0' || ch > '9') {
                invalid = true;
            }
        }

        if(invalid || uid.length() < 3) {
            new_uid = uid + key;
        } else {
            int diff = ch - '0';
            int pos = diff % 3;
            new_uid = uid.substr(0, pos) + key + uid.substr(pos, uid.length() - pos);
        }

        MD5 encoder(new_uid);
        return encoder.hexdigest();
    }
};
#endif
