#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "select_common.h"

int
parse_filter(const char *in_buffer, map<string, string> &filters, const char *const delims) {
    char *key = NULL;
    size_t key_len = 0;
    char *value = NULL;
    size_t value_len = 0;
    char *p = NULL;
    if(NULL != in_buffer) {
        char *buffer = (char *)in_buffer;
        //char *buffer = (char*)malloc(strlen(in_buffer) + 1);
        //memcpy(buffer, in_buffer, strlen(in_buffer) + 1);
        for(p = strchr(buffer, '='); p; p = strchr(p, '=')) {
            if(p == buffer) {
                ++p;
                continue;
            }
            for(key = p - 1; isspace(*key); --key);
            key_len = 0;
            while(isalnum(*key) || '_' == *key || '\\' == *key || '/' == *key || ':' == *key) {
                /* don't parse backwards off the start of the string */
                if(key == buffer) {
                    --key;
                    ++key_len;
                    break;
                }
                --key;
                ++key_len;
            }
            ++key;
            *(buffer + (key - buffer) + key_len) = '\0';
            for(value = p + 1; isspace(*value); ++value);
            value_len = strcspn(value, delims);
            p = value + value_len;
            if('\0' != *p) {
                *(value + value_len) = '\0';
                p = value + value_len + 1;
            } else {
                p = value + value_len;
            }
            filters.insert(make_pair(key, value));
        }
        /*
        if (NULL != buffer){
            free(buffer);
        }
        */
    }
    return 0;
}

int
parse_filter(string &raw_filter, list<string> &filters, const char *const delims) {
    char *key = NULL;
    size_t key_len = 0;
    char *p = (char *)raw_filter.c_str();
    if(NULL != p) {
        char *buffer = p;
        //char *buffer = (char*)malloc(strlen(p) + 1);
        //memcpy(buffer, p, strlen(p) + 1);
        for(p = buffer; p && *p; /*void*/) {
            for(key = p; isspace(*key); ++key);
            key_len = strcspn(key, delims);
            p = key + key_len;
            if('\0' != *p) {
                *(key + key_len) = '\0';
                p = key + key_len + 1;
            } else {
                p = key + key_len;
            }
            filters.push_back(key);
        }
        /*
        if (NULL != buffer){
            free(buffer);
        }
        */
    }
    return 0;
}

int
parse_filter(string &raw_filter, vector<string> &filters, const char *const delims) {
    const char *key = NULL;
    size_t key_len = 0;
    const char *p = raw_filter.c_str();
    if(NULL != p) {
        char *buffer = (char*)malloc(strlen(p) + 1);
        memcpy(buffer, p, strlen(p) + 1);
        for(p = buffer; p && *p; /*void*/) {
            for(key = p; isspace(*key); ++key);
            key_len = strcspn(key, delims);
            p = key + key_len;
            if('\0' != *p) {
                *(buffer + (key - buffer) + key_len) = '\0';
                p = key + key_len + 1;
            } else {
                p = key + key_len;
            }
            filters.push_back(key);
        }
        if(NULL != buffer) {
            free(buffer);
        }
    }
    return 0;
}

int
get_last_files_mod_time(list<string> &files, time_t &last_mod_time) {
    struct stat file_stat;
    for(list<string>::iterator iter = files.begin(); iter != files.end(); iter++) {
        if(0 != stat(iter->c_str(), &file_stat)) {
            return -1;
        }
        if(file_stat.st_mtime > last_mod_time) {
            last_mod_time = file_stat.st_mtime;
        }
    }
    return 0;
}

int
get_last_files_mod_time(string &file_name, time_t &last_mod_time) {
    struct stat file_stat;
    if(0 != stat(file_name.c_str(), &file_stat)) {
        return -1;
    }
    last_mod_time = file_stat.st_mtime;
    return 0;
}
