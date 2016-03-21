// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#ifndef DAS_LIB_DAS_INC_MANAGER_H
#define DAS_LIB_DAS_INC_MANAGER_H

#include <stdint.h>
#include <string>
#include <vector>

#include <cm_utility/non_copyable.h>

#include "das_inc_reader.h"     // for DasIncConf

namespace configio {
class DynamicRecord;
}

namespace das_lib {

// 多条DAS增量流的管理类，使用者在追增量时只需要调用read_next()接口
class DasIncManager {
public:
    DasIncManager() {}
    ~DasIncManager();
    
    // 按照添加的顺序依次读取各增量流的增量
    // 将一条增量流的全部增量处理完之后才读取下一个增量流
    int read_next(configio::DynamicRecord *p_record);

    bool add_das_inc(const DasIncConf &das_inc_conf);

    void set_max_reading_lines_for_all_inc(uint64_t num);

    // 将各inc reader的每轮读取行数设置成conf指定值
    void reset_max_reading_lines_for_all_inc();
    
private:
    struct DasIncObject {
        DASIncReader *p_inc_reader;   // own this object
        DasIncConf conf;
        std::string name;
    };

    DISALLOW_COPY_AND_ASSIGN(DasIncManager);

    void set_max_lines_per_round(uint32_t index, uint64_t num);

    //各条das增量流
    std::vector<DasIncObject> _das_inc_list;

};

} // das_lib
#endif // DAS_LIB_DAS_INC_MANAGER_H

