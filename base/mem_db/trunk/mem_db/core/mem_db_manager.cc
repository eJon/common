// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_manager.h
//      desc: implementation class MemDBManager
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/core/mem_db_manager.h>

#include <mem_db/core/mem_db_conf_def.h>
#include <mem_db/core/mem_db_engine.h>
#include <mem_db/core/mem_db_util.h>

namespace mem_db {

#define INTERVAL_FOR_CHECK_AUTO_LOAD_CONFIG_OPTION 10  // in seconds
#define INTERVAL_THRESHOLD_FOR_LOAD_CONFIG_OPTION  10 // in seconds


MemDBManager::MemDBManager() {
  mem_db_engine_ = NULL;
  slave_mem_db_engine_ = NULL;

  thread_stop_flag_ = false;  // do not stop thread
}

MemDBManager::~MemDBManager() {
  this->Free();
}

int MemDBManager::Init (const char *conf_file) {
  int ret = MEM_DB_SUCCESS;

  if (NULL != mem_db_engine_) {
    delete mem_db_engine_;
  }

  mem_db_engine_ = new MemDBEngine ();

  if (NULL == mem_db_engine_) {
    return MEM_DB_MEMORY_ERROR;
  }

  config_file_name_ = conf_file;

  ret = mem_db_engine_->Init (conf_file);
  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // start backend thread for auto-load-config feature
#ifdef  ENABLE_AUTO_LOAD_CONFIG
  ret = this->Run();
  if (0 != ret) {
    return ret;
  }
#endif

  return MEM_DB_SUCCESS;
}


void *MemDBManager::InternalProcess() {
  int ret = MEM_DB_SUCCESS;
  int auto_load_config = 0;
  int interval_for_auto_load_config = 0;

  MemDBEngine *tmp_mem_db_engine;

  if (config_file_name_.empty()) {
    return NULL;
  }

  while (true != thread_stop_flag_) {
    ret = LoadConfigFile (config_file_name_.c_str(), auto_load_config, interval_for_auto_load_config);
    if ((0 != ret) || (1 != auto_load_config)) {
      ::sleep(INTERVAL_FOR_CHECK_AUTO_LOAD_CONFIG_OPTION); 
      continue;
    }

    
    if (NULL != slave_mem_db_engine_) {
      slave_mem_db_engine_->Free();
      delete slave_mem_db_engine_;
    }
    slave_mem_db_engine_ = new MemDBEngine ();

    if (NULL != mem_db_engine_) {
      ret = slave_mem_db_engine_->Init(config_file_name_.c_str()); 
      if (0 == ret) {
        tmp_mem_db_engine = mem_db_engine_;
        mem_db_engine_ = slave_mem_db_engine_;
        slave_mem_db_engine_ = tmp_mem_db_engine;
      } else {
        continue;
      }
    } else {
      continue;
    }

    if (interval_for_auto_load_config < INTERVAL_THRESHOLD_FOR_LOAD_CONFIG_OPTION) {
      interval_for_auto_load_config = INTERVAL_THRESHOLD_FOR_LOAD_CONFIG_OPTION;
    }
    ::sleep(interval_for_auto_load_config);
  }

  return NULL;
}

int MemDBManager::Free (void) {
#ifdef ENABLE_AUTO_LOAD_CONFIG
  int ret = MEM_DB_SUCCESS;

  // set thread stop flag to stop the thread
  this->thread_stop_flag_ = true;

  ret = this->Stop();
  if (0 != ret) {
    return ret;
  }
#endif

  if (mem_db_engine_) {
    return mem_db_engine_->Free();
  }

  if (slave_mem_db_engine_) {
    return mem_db_engine_->Free();
  }

  return MEM_DB_SUCCESS;
}

}  // namespace mem_db

