// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#include "das_inc_manager.h"

#include "das_lib_log.h"

namespace das_lib {

DasIncManager::~DasIncManager()
{
    for (size_t i = 0; i < _das_inc_list.size(); ++i) {
        delete _das_inc_list[i].p_inc_reader;
        _das_inc_list[i].p_inc_reader = NULL;
    }
}

bool DasIncManager::add_das_inc(const DasIncConf &das_inc_conf)
{
    DASIncReader *p_reader = new (std::nothrow) DASIncReader();
    if (NULL == p_reader) {
        DL_LOG_FATAL("Fail to allocate das inc reader for %s", das_inc_conf.name.c_str());
        return false;
    }

    if (0 != p_reader->init(das_inc_conf)) {
        DL_LOG_FATAL("Fail to init das inc reader for %s", das_inc_conf.name.c_str());
        return false;
    }

    DasIncObject inc_obj = {.p_inc_reader = p_reader, 
                            .conf = das_inc_conf, 
                            .name = das_inc_conf.name
                            };
    _das_inc_list.push_back(inc_obj);

    return true;
}

int DasIncManager::read_next(configio::DynamicRecord *p_record)
{
    if (NULL == p_record) {
        DL_LOG_FATAL("p_record is NULL");
        return -1;
    }

    int ret = 0;
    for (size_t i = 0; i < _das_inc_list.size(); ++i) {
        if (NULL == _das_inc_list[i].p_inc_reader) {
            DL_LOG_FATAL("inc reader for inc [%s] is NULL", _das_inc_list[i].name.c_str());
            return -1;
        }
        ret = _das_inc_list[i].p_inc_reader->read_next(*p_record);
        if (ret < 0) {
            DL_LOG_FATAL("Fail to read %s inc", _das_inc_list[i].name.c_str());
            break;
        } else if (DASIncReader::READ_END == ret) {
            // we did not add log here
            continue;
        } else {
            // read something, break out
            break;
        }
    }

    return ret;
}

void DasIncManager::set_max_reading_lines_for_all_inc(uint64_t num) 
{
    for (size_t i = 0; i < _das_inc_list.size(); ++i) {
        set_max_lines_per_round(i, num);
    }
}

void DasIncManager::set_max_lines_per_round(uint32_t index, uint64_t num)
{
    if (index >= _das_inc_list.size()) {
        DL_LOG_FATAL("Fail to set max lines per round, index[%u] is out of range, max is %u", 
                index, _das_inc_list.size() - 1);
        return;
    }
    
    if (NULL == _das_inc_list[index].p_inc_reader) {
        DL_LOG_FATAL("inc reader for inc[%u][%s] is NULL", index, _das_inc_list[index].name.c_str());
        return;
    }

    _das_inc_list[index].p_inc_reader->set_max_lines_per_round(num);
}

void DasIncManager::reset_max_reading_lines_for_all_inc()
{
    for (size_t i = 0; i < _das_inc_list.size(); ++i) {
        set_max_lines_per_round(i, _das_inc_list[i].conf.max_lines_per_round);
    }
}

}

