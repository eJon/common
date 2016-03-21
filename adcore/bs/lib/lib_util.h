#ifndef DAS_LIB_LIB_UTIL_H
#define DAS_LIB_LIB_UTIL_H

#include <stdint.h>

#include "input_object.h"

namespace das_lib {

//@param:   _partition_num 分库总数
//@param:   _partition_idx 本机BS所属分库编号, 0为起始号
class PartitionArg {
public:
    PartitionArg() : _partition_idx(0), _partition_num(1) {}
 
    PartitionArg(uint32_t part_idx, uint32_t part_num) :
        _partition_idx(part_idx),
        _partition_num(part_num) {}

    PartitionArg& set_partition_num(uint32_t part_num) {
        _partition_num = part_num;
        return *this;
    }

    PartitionArg& set_partition_idx(uint32_t part_idx) {
        _partition_idx = part_idx;
        return *this;
    }
     
    bool match_partition_unit(uint32_t unit_id) const {
        return (_partition_num <= 1 || unit_id % _partition_num == _partition_idx);
    }

    uint32_t get_partition_idx() const {
        return _partition_idx;
    }

    uint32_t get_partition_num() const {
        return _partition_num;

    }
    
private:

    uint32_t _partition_idx;
    
    uint32_t _partition_num;
    
};

inline double MB (long mem_in_bytes) { return mem_in_bytes/1024.0/1024.0; }

inline uint64_t never_zero(uint64_t x)
{
    return 0 != x ? x : 1;
}

int get_and_check_uint32(
    const configio::DynamicRecord &record,
    int index, const std::string &name, uint32_t &value,
    uint32_t min_value , uint32_t max_value ,
    int vindex);

int get_and_check_int32(
    const configio::DynamicRecord &record,
    int index, const std::string &name, int32_t &value,
    int min_value, int max_value,
    int vindex);

int get_and_check_uint64(
    const configio::DynamicRecord &record,
    int index, const std::string &name, uint64_t &value,
    uint64_t min_value, uint64_t max_value);

int get_and_check_int64(
    const configio::DynamicRecord &record,
    int index, const std::string &name, int64_t &value,
    long long min_value, long long max_value );

int get_and_check_int64(
    const configio::DynamicRecord &record,
    int index, const std::string &name, int64_t &value,
    long long min_value, long long max_value,
    int vindex);


int get_string(const configio::DynamicRecord &record,
        int index, const std::string &name, std::string &value);

// 这个方法用于解析类似 id1|id2|id3的增量内容
bool get_multiple_uint32(std::vector<uint32_t>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name,
                           uint32_t min_value,
                           uint32_t max_value);

// 这个方法用于解析类似 id1|id2|id3的增量内容
bool get_multiple_int64(std::vector<int64_t>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name,
                           int64_t min_value,
                           int64_t max_value);


// 这个方法用于解析类似 id1|id2|id3的增量内容
bool get_multiple_string(std::vector<std::string>* p_vec,
                           const configio::DynamicRecord &record,
                           int column_idx,
                           const std::string &field_name);

// Get resident memory of current process
// Returns:
//   -1      -  Fail to get the memory
// otherwise -  Resident memory, in bytes
long resident_mem_of_process();

int fix_last_char(char * src);

//消除行结束符
void eliminate_new_line_char(char* line);

time_t get_file_mtime(const std::string& fpath);

void write_using(const std::string& dir, int idx);

bool compose_lastest_base(const std::string& myidx_path,
            const std::string& mydone_name,
            int& mydir_no,
            std::string& myusing_idx_path);

}//namespace

#endif
