// Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com


#include "lib_util.h"

#include <limits.h>

#include <cm_utility/string_utils.h>    // for string_printf

#include "das_lib_log.h"

namespace das_lib {

// 基准目录0/1切换
static const int BASE_DIR_NUM = 2;

int get_and_check_uint32(const configio::DynamicRecord &record,
        int index, const std::string &name, uint32_t &value,
        uint32_t min_value, uint32_t max_value,
        int vindex)
{
    if (0 != record.get_uint32(index, value, vindex, NULL)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    if (value < min_value || value > max_value) {
        DL_LOG_WARNING("value of [%s] is [%u], not in scope [%u, %u]",
                name.c_str(), value, min_value, max_value);
        return -1;
    }

    return 0;
}

int get_and_check_int32(const configio::DynamicRecord &record,
        int index, const std::string &name, int32_t &value,
        int min_value, int max_value,
        int vindex)
{
    if (0 != record.get_int32(index, value, vindex, NULL)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    if (value < min_value || value > max_value) {
        DL_LOG_WARNING("value of [%s] is [%d], not in scope [%d, %d]",
                name.c_str(), value, min_value, max_value);
        return -1;
    }

    return 0;
}

int get_and_check_uint64(const configio::DynamicRecord &record,
        int index, const std::string &name, uint64_t &value,
        uint64_t min_value, uint64_t max_value)
{
    // Very very ugly workaround for very very ugly interface of configio! God damn it!
    unsigned long long v = 0;
    if (0 != record.get_uint64(index, v)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    value = v;
    if (value < min_value || value > max_value) {
        DL_LOG_WARNING("value of [%s] is [%lu], not in scope [%lu, %lu]",
                name.c_str(), value, min_value, max_value);
        return -1;
    }

    return 0;
}

int get_and_check_int64(const configio::DynamicRecord &record,
        int index, const std::string &name, int64_t &value,
        long long min_value, long long max_value,
        int vindex)
{
    long long v = 0;
    if (0 != record.get_int64(index, v, vindex, NULL)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    value = v;
    if (value < min_value || value > max_value) {
        DL_LOG_WARNING("value of [%s] is [%lld], not in scope [%lld, %lld]",
                name.c_str(), value, min_value, max_value);
        return -1;
    }

    return 0;
}

int get_and_check_int64(const configio::DynamicRecord &record,
        int index, const std::string &name, int64_t &value,
        long long min_value, long long max_value)
{
    long long v = 0;
    if (0 != record.get_int64(index, v)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    value = v;
    if (value < min_value || value > max_value) {
        DL_LOG_WARNING("value of [%s] is [%lld], not in scope [%lld, %lld]",
                name.c_str(), value, min_value, max_value);
        return -1;
    }

    return 0;
}

int get_string(const configio::DynamicRecord &record,
        int index, const std::string &name, std::string &value)
{
    if (0 != record.get_string(index, value)) {
        DL_LOG_WARNING("fail to get [%s] from DynamicRecord", name.c_str());
        return -1;
    }
    return 0;
}

bool get_multiple_uint32(std::vector<uint32_t>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name,
                           uint32_t min_value,
                           uint32_t max_value)
{
    if (NULL == p_vec) {
        DL_LOG_FATAL("p_vec is NULL");
        return false;
    }
    p_vec->clear();

    uint32_t id = 0;
    int count = record.get_field_count(column_idx);
    
    for (int i = 0; i < count; i++) {
        if (0 != get_and_check_uint32(record, column_idx, field_name, id, min_value, max_value, i)) {
            return false;
        }
        p_vec->push_back(id);
    }

    return true;
}

bool get_multiple_int64(std::vector<int64_t>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name,
                           int64_t min_value,
                           int64_t max_value)
{
    if (NULL == p_vec) {
        DL_LOG_FATAL("p_vec is NULL");
        return false;
    }
    p_vec->clear();

    int64_t id = 0;
    int count = record.get_field_count(column_idx);
    
    for (int i = 0; i < count; i++) {
        if (0 != get_and_check_int64(record, column_idx, field_name, id, min_value, max_value, i)) {
            return false;
        }
        p_vec->push_back(id);
    }

    return true;
}


bool get_multiple_string(std::vector<std::string>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name)
{
    if (NULL == p_vec) {
        DL_LOG_FATAL("p_vec is NULL");
        return false;
    }
    p_vec->clear();

    std::string id;
    int count = record.get_field_count(column_idx);
    
    for (int i = 0; i < count; i++) {
        if (0 != record.get_string(column_idx, id,i,NULL)) {
            DL_LOG_WARNING("fail to get [%s] from DynamicRecord,idx[%d]", field_name.c_str(),i);
            return false;
        }
        p_vec->push_back(id);
    }

    return true;
}

int fix_last_char(char * src)
{
    char flag = 0;
    while (* src) {
        if (* src & 0x80) { //碰到结尾符号
            flag = (flag) ? 0 : 1;
        } else {
            flag = 0;
        }
        src++;
    }

    if (flag) {
        *(--src) = 0;
        return 1;
    } else {
        return 0;
    }
}

long resident_mem_of_process()
{
// From man 5 proc:                                                         
// /proc/[number]/statm                                                     
//     Provides information about memory status in pages.  The columns are: 
//     size       total program size                                        
//     resident   resident set size                                         
//     share      shared pages                                              
//     trs        text (code)                                               
//     drs        data/stack                                                
//     lrs        library                                                   
//     dt         dirty pages                                               
    FILE* fp = fopen("/proc/self/statm", "r");                                  
    if (NULL == fp) {
        return -1L;
    }                                                                           
    long size, resident, share, trs, drs, lrs, dt;
    fscanf(fp, "%ld %ld %ld %ld %ld %ld %ld",     
       &size, &resident, &share, &trs, &drs, &lrs, &dt);
    fclose(fp);
    return resident * getpagesize();                } 

void eliminate_new_line_char(char* line)
{
    int str_len = strlen(line);
    if (str_len >= 2 && '\r' == line[str_len - 2]) {
        line[str_len - 2] = 0;
    } else if (str_len >= 1 && '\n' == line[str_len - 1]) {
        line[str_len - 1] = 0;
    }
    return;
}

time_t get_file_mtime(const std::string& fpath)
{
    const time_t ZERO_TM = {0};
    struct stat st;

    if (0 == stat(fpath.c_str(), &st) && (st.st_mode & S_IFREG)) {
        return st.st_mtime;
    } else {
        return ZERO_TM;
    }
}

void write_using(const std::string& dir, int idx)
{
    const int MAX_DIR_LEN = 1024;
    char path[MAX_DIR_LEN];
    snprintf(path, sizeof(path), "%s/using", dir.c_str());
    FILE* fp = fopen(path, "w");
    if (NULL == fp) {
        DL_LOG_FATAL("Fail to fopen `%s'", path);
        return;
    }
    fprintf(fp, "%d", idx);
    fclose(fp);
}

bool compose_lastest_base(const std::string& myidx_path,
                const std::string& mydone_name,
                int& mydir_no,
                std::string& myusing_idx_path)
{
    std::string mydone_fpath;
    time_t max_tm = {0};
    mydir_no = 0;
    for (int i = 0; i < BASE_DIR_NUM; ++i) {
        if (0 != cm_utility::string_printf(&mydone_fpath, "%s/%d/%s", myidx_path.c_str(), i, mydone_name.c_str())) {
            DL_LOG_FATAL("Fail to printf mydone_fpath");
            return false;
        }
        time_t tm = get_file_mtime(mydone_fpath);
        if (tm > max_tm) {
            max_tm = tm;
            mydir_no = i;
        }
    }
    if (max_tm == 0) {
        DL_LOG_FATAL("Fail to find file '%s' in %s/%d or %s/%d",
            mydone_name.c_str(), myidx_path.c_str(), 0, myidx_path.c_str(), 1);
        return false;
    }

    if (0 != cm_utility::string_printf(&myusing_idx_path,"%s/%d", myidx_path.c_str(), mydir_no) ) {
        DL_LOG_FATAL("Failed to printf string");
        return false;
    }
    DL_LOG_MON("Using base indexes at %s", myusing_idx_path.c_str());

    write_using(myidx_path, mydir_no);
    return true;
}

}//namespace
