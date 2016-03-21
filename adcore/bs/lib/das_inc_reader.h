// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: 封装了configio类，通过配置提供读取基准和增量的服务

#ifndef DAS_LIB_DAS_INC_READER_H
#define DAS_LIB_DAS_INC_READER_H

#include <vector>
#include <map>
#include <dirent.h>
#include "configio.h"

namespace das_lib {

struct DasBaseConf {
public:
    std::string name;
    std::string configio_xml_path;
    std::string base_dir;
    unsigned long long last_event_id;
    
    DasBaseConf()
        : last_event_id(0)
    {
    }
};

struct DasIncConf {
    std::string name;
    std::string configio_xml_path;
    unsigned long long last_event_id;
    uint64_t max_lines_per_round;
    std::string pipe_name;
};

struct DasTopicInfo {
    std::string cur_file;
    uint64_t cur_line;
};
 
class DASIncReader {
public:
    DASIncReader();

    ~DASIncReader();

    int init(const DasBaseConf &conf);
    int init(const DasIncConf &conf);

    int read_next(configio::DynamicRecord &record);

    void set_max_lines_per_round(uint64_t num);

    int close();

    enum read_result_t {
        READ_FAIL = -1,
        READ_SUCC = 0,
        READ_END = 1
    };

    bool is_end_of_reader();

    void get_topic_info(DasTopicInfo& info) const;
    
private:

    configio::InputObject _reader;

    uint64_t _start_event_ids;
    bool _has_sought;  // 是否已经成功seek到了正确的位置
    uint64_t _last_event_ids;

    uint64_t _max_lines_per_round;

    // 当前的reader已经读取的行数
    uint64_t _cur_lines;

    std::string _desc;
};

}//das lib

#endif // DAS_LIB_DAS_INC_READER_H
