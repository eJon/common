#include "das_inc_reader.h"
#include "das_lib_log.h"
#include "cm_utility/string_utils.h"

namespace das_lib {

DASIncReader::DASIncReader()
    : _start_event_ids(0)
    , _has_sought(false)
    , _last_event_ids(0)
    , _max_lines_per_round(ULLONG_MAX)
    , _cur_lines(0)
{
}

DASIncReader::~DASIncReader()
{
    close();
}

int DASIncReader::init(const DasIncConf &conf)
{
    int ret = 0;

    _max_lines_per_round = conf.max_lines_per_round;
    _cur_lines = 0;
    _desc = conf.pipe_name;
    
    ret = _reader.init(conf.configio_xml_path.c_str());
    if (0 != ret) {
        DL_LOG_FATAL("fail to init InputObject, conf[%s].",
                conf.configio_xml_path.c_str());
        return -1;
    }
    // open and seek.
    ret = _reader.open();
    if (0 != ret) {
        DL_LOG_FATAL("fail to open InputObject, conf[%s].",
                conf.configio_xml_path.c_str());
        return -1;
    }

    ret = _reader.seek(&conf.last_event_id, sizeof(conf.last_event_id));
    if (ret < 0) {
        DL_LOG_FATAL("fail to seek to event_id[%llu], conf[%s], ret[%d].",
                conf.last_event_id, conf.configio_xml_path.c_str(),ret);
        return -1;
    }
    _start_event_ids = conf.last_event_id;
    _has_sought = (0 == ret);

    _last_event_ids = conf.last_event_id;

    return 0;
}

int DASIncReader::init(const DasBaseConf &conf)
{
    int ret = 0;

    ret = _reader.init(conf.configio_xml_path.c_str());
    if (0 != ret) {
        DL_LOG_FATAL("fail to init InputObject, conf[%s].",
                      conf.configio_xml_path.c_str());
        return -1;
    }

    // reset file dir.
    // 基准文件夹有0/1切换，因此不能写在xml中
    std::string dir(conf.base_dir.c_str());
    ret = _reader.set_meta_info(
            configio::READER_LITERAL_FILE_LINE_CHILD_SRC_FILE_DIR,
            (void*)&dir);
    if (0 != ret) {
        DL_LOG_FATAL("fail to set meta[%s] of InputObject, conf[%s].",
                configio::READER_LITERAL_FILE_LINE_CHILD_SRC_FILE_DIR,
                conf.configio_xml_path.c_str());
        return -1;
    }
                
    // open.    
    ret = _reader.open();
    if (0 != ret) {
        DL_LOG_FATAL("fail to open InputObject, conf[%s].",
                      conf.configio_xml_path.c_str());
        return -1;
    }

    _start_event_ids = 0;
    _has_sought = true;
    _last_event_ids = 0;

    return 0;
}

void DASIncReader::set_max_lines_per_round(uint64_t num)
{
    _max_lines_per_round = num;
}

bool DASIncReader::is_end_of_reader()
{
    bool reach_end = false;
    _reader.get_meta_info(configio::READER_META_REACH_END, &reach_end);

    return reach_end;
}

int DASIncReader::read_next(configio::DynamicRecord &record)
{
    int ret = 0;
    unsigned long long cur_event_id;

    // check lines.
    if (_max_lines_per_round > 0 &&
        _cur_lines == _max_lines_per_round) {
        DL_LOG_TRACE("read enough lines[%d], max lines[%d]",
                _cur_lines, _max_lines_per_round);
        _cur_lines = 0;
        return READ_END;
    }
       
    // read next record.
    ret = _reader.get_next_record(record);

    if (ret < 0) {
        return READ_FAIL;
    }

    if (ret == 1) {
        _cur_lines = 0;
        return READ_END;
    }

    // check event id.
    ret = record.get_uint64(1, cur_event_id);
    if (0 != ret) {
        DL_LOG_FATAL("fail to get event id from record");
        return -1;
    }

    if (cur_event_id < _last_event_ids) {
        DL_LOG_FATAL("current_event_id[%llu] is less than previous[%llu]",
                cur_event_id,
                _last_event_ids);
        return -1;
    }
            
    _last_event_ids = cur_event_id;
    _cur_lines++;
           
    return 0;

}

int DASIncReader::close()
{
    _reader.close();

    return 0;
}

void DASIncReader::get_topic_info(DasTopicInfo& info) const {

    std::string cur_file_name;
    _reader.get_meta_info(
            configio::READER_META_CUR_FULL_FILE_NAME, (void*)&cur_file_name);
    _reader.get_meta_info(
                configio::READER_META_CUR_LINE_NO, (void*)&info.cur_line);
    info.cur_file = cur_file_name.c_str();
}

}//das lib
