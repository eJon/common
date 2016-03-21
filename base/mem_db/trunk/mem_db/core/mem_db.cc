// copyright:
//            (C) SINA Inc.
//
//      file: mem_db.h
//      desc: implementation class MemDB
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/mem_db.h>
#include <mem_db/core/mem_db_manager.h>
#include <mem_db/core/mem_db_engine.h>

namespace mem_db {

MemDB::MemDB () : mem_db_manager_ (NULL) {
}

MemDB::~MemDB () {
  this->Free ();
}

int MemDB::Init (const char *conf_file) {
  if (NULL != mem_db_manager_) {
    delete mem_db_manager_;
  }

  mem_db_manager_ = new MemDBManager ();

  if (NULL == mem_db_manager_) {
    return MEM_DB_MEMORY_ERROR;
  }

  return mem_db_manager_->Init (conf_file);
}

int MemDB::Init (const std::string &conf_file) {
  return this->Init (conf_file.c_str ());
}

int MemDB::Free (void) {
  if (NULL != mem_db_manager_) {
    int ret = mem_db_manager_->Free ();

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    mem_db_manager_ = NULL;
  }

  return MEM_DB_SUCCESS;
}

int MemDB::ListHosts (std::vector<HostInfo> &hosts) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->ListHosts (hosts);
}

int MemDB::GetGenenalValue (const std::string &key, std::string &value) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetGeneralValue (key, value);
}

int MemDB::GetGeneralValue (const std::string &key, std::string &value) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetGeneralValue (key, value);
}

int MemDB::MultiGetGeneralValue (const std::vector<std::string> &keys,
                                 std::vector<std::string> &values) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->MultiGetGeneralValue (keys, values);
}


int MemDB::SetGeneralValue (const std::string &key, const std::string &value, int expiration_time) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->SetGeneralValue (key, value, expiration_time);
}

int MemDB::DeleteGeneralValue (const std::string &key) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->DeleteGeneralValue (key);
}

int MemDB::ListTables (std::vector<std::string> &tables) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->ListTables (tables);
}

int MemDB::GetColumnDescriptors (const std::string &table,
                                 std::vector<ColumnDesp> &columns) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetColumnDescriptors (table, columns);
}

int MemDB::CreateTable (const std::string &table,
                        const std::vector<ColumnDesp> &columns) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->CreateTable (table, columns);
}

int MemDB::DeleteTable (const std::string &table) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->DeleteTable (table);
}

int MemDB::AddColumns (const std::string &table,
                       const std::vector<ColumnDesp> &columns) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->AddColumns (table, columns);
}

int MemDB::GetRow (const std::string &table,
                   const std::string &key,
                   std::map<std::string, std::string> &row_record) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetRow (table, key, row_record);
}

int MemDB::GetRowWithColumns (const std::string &table,
                              const std::string &key,
                              const std::vector<std::string> &columns,
                              std::vector<std::string> &values) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetRowWithColumns (table, key, columns, values);
}

int MemDB::GetRows (const std::string &table,
                    const std::vector<std::string> &keys,
                    std::vector<std::map<std::string, std::string> > &row_records) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetRows (table, keys, row_records);
}

int MemDB::GetRowsWithColumns (const std::string &table,
                               const std::vector<std::string> &keys,
                               const std::vector<std::string> &columns,
                               std::vector<std::vector<std::string> > &values) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetRowsWithColumns (table, keys, columns, values);
}

int MemDB::PutRow (const std::string &table,
                   const std::string &key,
                   const std::map<std::string, std::string> &row_record) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->PutRow (table, key, row_record);
}

int MemDB::PutRowWithColumns (const std::string &table,
                              const std::string &key,
                              const std::vector<std::string> &columns,
                              const std::vector<std::string> &values) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->PutRowWithColumns (table, key, columns, values);
}

int MemDB::PutRows (const std::string &table,
                    const std::vector<std::string> &keys,
                    const std::vector<std::map<std::string, std::string> > &row_records) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->PutRows (table, keys, row_records);
}

int MemDB::PutRowsWithColumns (const std::string &table,
                               const std::vector<std::string> &keys,
                               const std::vector<std::string> &columns,
                               const std::vector<std::vector<std::string> > &values) {

  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->PutRowsWithColumns (table, keys, columns, values);
}

int MemDB::GetTables (const std::vector<MutationTable> &mutation,
                      std::vector<std::vector<std::vector<std::string> > > &table_contents) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->GetTables (mutation, table_contents);
}

int MemDB::SetTables (const std::vector<MutationTable> &mutation,
                      const std::vector<std::vector<std::vector<std::string> > > &table_contents) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->SetTables (mutation, table_contents);
}

int MemDB::DeleteRow (const std::string &table, const std::string &key) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->DeleteRow (table, key);
}

int MemDB::DeleteRowWithColumns (const std::string &table,
                                 const std::string &key,
                                 const std::vector<std::string> &columns) {
  if ((NULL == mem_db_manager_) || (NULL == mem_db_manager_->mem_db_engine())) {
    return MEM_DB_INVALID_DATA;
  }

  return mem_db_manager_->mem_db_engine()->DeleteRowWithColumns (table, key, columns);
}

}  // namespace mem_db
