// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_manager.h
//      desc: manager for mem_db
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#ifndef MEM_DB_MANAGER_H_
#define MEM_DB_MANAGER_H_

#include <mem_db/common/thread_base.h>
#include <mem_db/core/mem_db_def_internal.h>

namespace mem_db {

// forward declaration
class MemDBEngine;

class MemDBManager : mem_db_util::ThreadBase {
  public:
    MemDBManager();
    ~MemDBManager();
    int Init (const char *conf_file);
    int Free (void);

    MemDBEngine *mem_db_engine() {
      return this->mem_db_engine_;
    }

  private:
    void *InternalProcess();
    std::string config_file_name_;
    bool thread_stop_flag_;

    MemDBEngine *mem_db_engine_; 
    MemDBEngine *slave_mem_db_engine_; 

    // no copy and assignment
    DISALLOW_COPY_AND_ASSIGN (MemDBManager);
};

}  // namespace mem_db

#endif  // MEM_DB_MANAGER_H_
