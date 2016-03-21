// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: lib 管理器的基类，根据基准和增量维护tables,并提供lib中所有table的更新，查找操作。
// 默认实现了load及handle inc方法,并通过MVCC提供更新时的读写保护

#include "lib_manager.h"
#include <limits.h>

#include <Configure.h>

#include "das_lib_log.h"

using st::VersionManager;
using configio::DynamicRecord;

namespace das_lib {

//field index in config io records
//common level
const int INC_EVENT_ID_IDX = 1;
const int INC_LEVEL_IDX = 3;    

typedef uint64_t event_id_t;

LibManagerBase::LibManagerBase()
{}

LibManagerBase::~LibManagerBase()
{}

TableGroup* LibManagerBase::latest_table_group() 
{
    int pos = _vm_tg.find_latest_read_only();
    if (pos < 0) {
        DL_LOG_FATAL("Failed to find ready on vm");
        return NULL;
    }
    return &_vm_tg[pos];
}

TableGroup *LibManagerBase::create_version(int *pos)
{
    if (NULL == pos) {
        DL_LOG_FATAL("pos is NULL");
        return NULL;
    }
    
    st::Timer tm;
    tm.start();

    DL_LOG_MON("Begin creating new version of main_table_group");

    *pos = _vm_tg.create_version();
    if (*pos < 0) {
        DL_LOG_FATAL("Fail to create a new version of main_table_group");
        return NULL;
    }

    TableGroup* p_table_group = &_vm_tg[*pos];

    tm.stop();
    DL_LOG_MON("End creating new version of main_table_group: %lums",
            tm.m_elapsed());
    return p_table_group;
}

bool LibManagerBase::init(const comcfg::ConfigUnit& config)
{
    if (!load_conf(config)) {
        DL_LOG_FATAL("Failed to load conf");
        return false;
    }

    int ret = _vm_tg.init(TABLE_VERSION_NUM, "table_group_version_manager");
    if (ret < 0) {
        DL_LOG_FATAL("Fail to init table group version manager");
        return false;
    }
    
    //注册table
    int pos = _vm_tg.create_version();
    if (pos < 0) {
        DL_LOG_FATAL("Failed to create table group verion");
        return false;
    }

    if (!register_tables(&(_vm_tg[pos]))) {
        DL_LOG_FATAL("Failed to register tables");
        return false;
    }

    if (!_vm_tg[pos].init()) {
        DL_LOG_FATAL("Fail to init table group");
        return false;
    }
    
    _vm_tg.freeze_version(pos);

    if (_dump_req_watcher.create("dump_req") < 0) {
        DL_LOG_FATAL("Fail to create file_watcher for dump_req");
        return false;
    }

    return true;
}

bool LibManagerBase::add_das_inc(const DasIncConf &das_inc_conf)
{
    return _inc_manager.add_das_inc(das_inc_conf);
}

bool LibManagerBase::add_literal_base_conf(const DasBaseConf &das_base_conf)
{
    _literal_base_conf_list.push_back(das_base_conf);

    return true;
}

bool LibManagerBase::start_service()
{
    bool ret = load_base_indexes();
    if (!ret) {
        DL_LOG_FATAL("Failed to load base indexes");
        return false;
    }

    //启动时追增量，为了追完所有增量，不能限制每轮追增量的个数
    _inc_manager.set_max_reading_lines_for_all_inc(ULLONG_MAX);

    ret = handle_inc();
    if (!ret) {
        DL_LOG_FATAL("Failed to load inc before service");
        return false;
    }

    _inc_manager.reset_max_reading_lines_for_all_inc();

    return true;
}

bool LibManagerBase::load_base_indexes()
{
    int pos = -1;
    TableGroup *p_table_group = create_version(&pos);
    if (NULL == p_table_group) {
        DL_LOG_FATAL("Fail to create version of table group");
        return false;
    }

    DL_LOG_MON("Begin to load base indexes");

    st::Timer tm;
    tm.start();
    
    if (!p_table_group->pre_load()) {
        _vm_tg.drop_version(pos);
        DL_LOG_FATAL("Failed to pre load table");
        return false;
    }

    // 先加载其他方式的基准
    if (!p_table_group->load()) {
        _vm_tg.drop_version(pos);
        DL_LOG_FATAL("Failed to load table");
        return false;
    }

    // 加载各文本基准
    for (size_t i = 0; i < _literal_base_conf_list.size(); ++i) {
        if (!load_literal_base(_literal_base_conf_list[i], p_table_group)) {
            _vm_tg.drop_version(pos);
            DL_LOG_FATAL("Failed to load literal base");
            return false;
        }
    }
    
    if (!p_table_group->post_load()) {
        _vm_tg.drop_version(pos);
        DL_LOG_FATAL();
        return false;
    }

    _vm_tg.freeze_version(pos);

    tm.stop();

    DL_LOG_MON("Load base end time[%lu]s", tm.elapsed());

    return true;
}

bool LibManagerBase::load_literal_base(const DasBaseConf &conf,
                                        TableGroup* p_table_group)
{
    // das的文本基准和增量格式一样，因此使用追增量的逻辑处理基准
    if (NULL == p_table_group) {
        DL_LOG_FATAL("p_table_group is NULL");
        return false;
    }

    DL_LOG_MON("Begin loading literal base [%s]", conf.name.c_str());

    st::Timer tm;
    tm.start();

    DASIncReader base_reader; 
    if (0 != base_reader.init(conf)) {
        DL_LOG_FATAL("Fail to init das base reader for %s", conf.name.c_str());
        return false;
    }

    _inc_count_map.clear();
    
    int ret = 0;
    event_id_t last_event_id = 0;
    event_id_t cur_eid = 0;
    uint32_t count = 0;
    uint32_t level = 0;
    configio::DynamicRecord record;
    
    while (true) {
        // get next record
        ret = base_reader.read_next(record);
        if (ret < 0) {
            DL_LOG_FATAL("Failed to read next das inc");
            continue;
        } else if (DASIncReader::READ_END == ret) {
            break;
        }
        
        count ++;
        if (0 != get_and_check_uint64(record, INC_EVENT_ID_IDX, "event_id", cur_eid, 0UL, ULONG_MAX)) {
            DL_LOG_FATAL("Failed to get event id");
            continue;
        }

        if (0 != get_and_check_uint32(record, INC_LEVEL_IDX, "level", level, 0,UINT_MAX,-1)) {
            DL_LOG_FATAL("Failed to get level");
            continue;
        }

        //记录每个层级的增量个数
        _inc_count_map[level] ++;

        if (!p_table_group->handle_inc(record, level)) {
            DL_LOG_FATAL("Failed to handle_inc, event_id=%llu", cur_eid);
            continue;
        }

        last_event_id = cur_eid;
    }

    DL_LOG_MON("handled %u incs, last event id is %llu", count, last_event_id);
    
    tm.stop();
    DL_LOG_MON("End loading literal base [%s], used time[%lu]s", conf.name.c_str(), tm.elapsed()); 

    return true;
}

bool LibManagerBase::handle_inc()
{
    dump_table_if_required();

    bool is_need_reload = false;
    //检查是否有动态词表需要reload
    for (size_t i = 0; i < _file_watcher_list.size(); ++i) {
        if (NULL == _file_watcher_list[i]) {
            DL_LOG_FATAL("NULL file watcher found at %u", i);
            return false;
        } else if (_file_watcher_list[i]->is_timestamp_updated() > 0) {
            is_need_reload = true;
            break;
        }
    }

    //检查是否有增量
    bool has_inc = false;
    configio::DynamicRecord record;
    int ret = _inc_manager.read_next(&record);
    if (ret < 0) {
        DL_LOG_FATAL("Fail to read first inc");
        return false;
    } else if (DASIncReader::READ_END == ret) {
        DL_LOG_MON("No DAS inc to handle since last round");
    } else {
        has_inc = true;
    }

    TableGroup *p_table_group = NULL;
    int pos = -1;
    if (is_need_reload || has_inc) {
        p_table_group = create_version(&pos);
        if (NULL == p_table_group) {
            DL_LOG_FATAL("Fail to create version of table group");
            return false;
        }
    } else {
        //既没有增量也没有动态词表更新
        DL_LOG_MON("there is no file updated nor inc");
        return true;
    }

    if (is_need_reload) {
        if (!p_table_group->reload()) {
            DL_LOG_FATAL("Fail to reload tables");
            _vm_tg.drop_version(pos);
            return false;
        }
    }

    if (has_inc) {
        DL_LOG_MON("Begin to handle inc");
        if (!handle_inc_impl(record, p_table_group)) {
            DL_LOG_FATAL("Failed to handle inc");
            _vm_tg.drop_version(pos);
            return false;
        }
    }
    
    _vm_tg.freeze_version(pos);

    return true;
}

bool LibManagerBase::handle_inc_impl(configio::DynamicRecord& record, TableGroup* p_table_group)
{
    st::Timer tm;
    tm.start();

    if (NULL == p_table_group) {
        DL_LOG_FATAL("p_table_group is NULL");
        return false;
    }
    
    int ret = 0;
    event_id_t last_event_id = 0;
    event_id_t cur_eid = 0;
    uint32_t count = 0;
    uint32_t level = 0;

    //清零计数器
    _inc_count_map.clear();
    
    do {
        // get current event id.
        count ++;
        if (0 != get_and_check_uint64(record, INC_EVENT_ID_IDX, "event_id", cur_eid, 0UL, ULONG_MAX)) {
            DL_LOG_FATAL("Failed to get event id");
            continue;
        }

        if (0 != get_and_check_uint32(record, INC_LEVEL_IDX, "level", level, 0, UINT_MAX, -1)) {
            DL_LOG_FATAL("Failed to get level");
            continue;
        }

        //记录每个层级的增量个数
        _inc_count_map[level] ++;

        if (!p_table_group->handle_inc(record, level)) {
            DL_LOG_FATAL("Failed to handle_inc, event_id=%llu", cur_eid);
        }
        
        last_event_id = cur_eid;

        // get next record
        ret = _inc_manager.read_next(&record);
        if (ret < 0) {
            DL_LOG_FATAL("Failed to read next das inc");
            continue;
        } else if (DASIncReader::READ_END == ret) {
            break;
        }

    } while (true);

    DL_LOG_MON("In handle inc, handled %u incs, last event id is %llu", count, last_event_id);

    log_handle_inc_info(p_table_group);
    
    tm.stop();
    DL_LOG_MON("End inc 1 round,handled time[%lu]ms",tm.elapsed()); 

    return true;
}

void LibManagerBase::dump_table_if_required()
{
    if (_dump_req_watcher.is_timestamp_updated() > 0) {
        do {
            char buf[256];
            const size_t LEN = sizeof(buf) / sizeof(*buf);

            FILE* fp = fopen(_dump_req_watcher.filepath(), "r");
            if (NULL == fp) {
                DL_LOG_FATAL("Fail to open %s", _dump_req_watcher.filepath());
                break;
            }
            fgets(buf, LEN, fp);

            int len = strlen(buf);
            buf[len-1] = '\0';

            fclose(fp);

            DL_LOG_MON("Detected that %s is updated, start to dump(%s)",
                    _dump_req_watcher.filepath(),buf);
            
            int pos = _vm_tg.find_latest_read_only();
            if (pos < 0) {
                 DL_LOG_FATAL("Fail to find a read-only main_table_group");
                 return;
            }

            std::string dump_dir = "dump";
            mkdir(dump_dir.c_str(), 0777);

            if (0 == strncmp(buf,"all",len)) {
                _vm_tg[pos].serialize(dump_dir);
            } else {
                _vm_tg[pos].serialize(dump_dir, buf);
            }
            

            _dump_req_watcher.update_timestamp();
        } while (0);
    }
}

void LibManagerBase::log_handle_inc_info(TableGroup* p_table_group) const 
{
    //首先输出增量的信息
    DL_LOG_MON("+-------------------------------------------------------+");
    DL_LOG_MON("| inc level          | count                            |");
    DL_LOG_MON("|--------------------+----------------------------------|");

    IncCountMap::const_iterator cnt_iter = 
        _inc_count_map.begin();
    for (;cnt_iter != _inc_count_map.end(); ++cnt_iter) {
        DL_LOG_MON("| %-15d | %-10.2d | ",
                cnt_iter.key(),cnt_iter.value());
    }

    DL_LOG_MON("+-------------------------------------------------------+");

    //再打印各个table的信息
    p_table_group->log_table_info();
}

void LibManagerBase::add_file_watcher(cm_utility::FileWatcher* fw) 
{
    _file_watcher_list.push_back(fw);
}

}  // namespace das_lib
