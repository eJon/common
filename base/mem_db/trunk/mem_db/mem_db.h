// copyright:
//            (C) SINA Inc.
//
//      file: mem_db.h
//      desc: global header file for mem_db library
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2012-10-10
//
//    change: 2014-02-17: udpate comments

#ifndef MEM_DB_H_
#define MEM_DB_H_

#include <map>
#include <string>
#include <vector>

#include <mem_db/mem_db_def.h>

namespace mem_db {

class MemDBManager;

// class MemDB is the main interface for adoption; it is thread-safe,
// but each thread should have it's own MemDB instance. And it is not
// assignable nor copiable.
class MemDB {
  public:
    // Description:
    //   - Constructor.
    MemDB ();
    // Description:
    //   - Deconstructor.
    ~MemDB ();

    // Description:
    //   - Initialzation function for MemDB.
    //
    // params:
    //   - conf_file: configuration file name(including directory).
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int Init (const char *conf_file);
    int Init (const std::string &conf_file);

    // Description:
    //   - Release function for MemDB.
    // return:
    //    - 0 on success;
    //    - non-0 on failure.
    int Free (void);

    // Description:
    //    - Get info. for storage server nodes.
    //
    // params:
    //    - hosts: returned info for all server nodes.
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int ListHosts (std::vector<HostInfo> &hosts);

    // Description:
    //   - This API should NEVER be used again.
    DEPRECATED int GetGenenalValue (const std::string &key, std::string &value);

    // Description:
    //   - To get value with key from storage for k-v pairs. If the value does not
    //     exist, empty string "" will be returned. Notice it can not define whether
    //     empty value with an associated key or the key-value does not exist at all.
    //
    // params:
    //   - key: key string;
    //   - value: returned value string.
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int GetGeneralValue (const std::string &key, std::string &value);
    // Description:
    //   - To get values with associated keys in MGET mode from storage for k-v pairs.
    //     Length of vector values have the same length with length of keys.
    //
    // params:
    //   - key: vector for key strings;
    //   - value: returned vector for value strings.
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int MultiGetGeneralValue (const std::vector<std::string> &keys, std::vector<std::string> &values);
    // Description:
    //   - To store value with associated key into storage for k-v pairs.
    //
    // params:
    //   - key: key for k-v pair to storge;
    //   - value: value for k-v pair to storage;
    //   - expiration_time: expiration time in seconds for k-v pair to store.
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int SetGeneralValue (const std::string &key, const std::string &value, int expiration_time = 0);
    // Description:
    //   - To delete value with associated key from storage for k-v pairs.
    //
    // params:
    //   - key: key for k-v pair to delete.
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int DeleteGeneralValue (const std::string &key);

    // The following two APIs are main to operate on table-item stoarge
    // Description:
    //   - To get values with associated table, key and columns from storage for table items.
    //
    // params:
    //   - table: table name for the table items to get; 
    //   - key: table key(key-like in database) for the table items to get;
    //   - columns: table column names for the table items to get;
    //   - values: value string the table items to get with assicated table, key and columns
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int GetRowWithColumns (const std::string &table,
                           const std::string &key,
                           const std::vector<std::string> &columns,
                           std::vector<std::string> &values);
    // Description:
    //   - To get values with associated table, key and columns from storage for table items.
    //
    // params:
    //   - table: table name for the table items to store;
    //   - key: table key(key-like in database) for the table items to store;
    //   - columns: table column names for the table items to store;
    //   - values: value string the table items to store with assicated table, key and columns
    // return:
    //   - 0 on success;
    //   - non-0 on failure.
    int PutRowWithColumns (const std::string &table,
                           const std::string &key,
                           const std::vector<std::string> &columns,
                           const std::vector<std::string> &values);

    int ListTables (std::vector<std::string> &tables);
    int GetColumnDescriptors (const std::string &table,
                              std::vector<ColumnDesp> &columns);
    int CreateTable (const std::string &table,
                     const std::vector<ColumnDesp> &columns);
    int DeleteTable (const std::string &table);
    int AddColumns (const std::string &table,
                    const std::vector<ColumnDesp> &columns);
    int GetRow (const std::string &table,
                const std::string &key,
                std::map<std::string, std::string> &row_record);
    int GetRows (const std::string &table,
                 const std::vector<std::string> &keys,
                 std::vector<std::map<std::string, std::string> > &row_records);
    int GetRowsWithColumns (const std::string &table,
                            const std::vector<std::string> &keys,
                            const std::vector<std::string> &columns,
                            std::vector<std::vector<std::string> > &values);
    int PutRow (const std::string &table,
                const std::string &key,
                const std::map<std::string, std::string> &row_record);
    int PutRows (const std::string &table,
                 const std::vector<std::string> &keys,
                 const std::vector<std::map<std::string, std::string> > &row_records);
    int PutRowsWithColumns (const std::string &table,
                            const std::vector<std::string> &keys,
                            const std::vector<std::string> &columns,
                            const std::vector<std::vector<std::string> > &values);
    int GetTables (const std::vector<MutationTable> &mutation,
                   std::vector<std::vector<std::vector<std::string> > > &table_contents);
    int SetTables (const std::vector<MutationTable> &mutation,
                   const std::vector<std::vector<std::vector<std::string> > > &table_contents);
    int DeleteRow (const std::string &table, const std::string &key);
    int DeleteRowWithColumns (const std::string &table,
                              const std::string &key,
                              const std::vector<std::string> &columns);
  private:
    MemDBManager *mem_db_manager_;  // mem_db manager handler

    // no copy and assignment
    DISALLOW_COPY_AND_ASSIGN (MemDB);

};

}  // namespace mem_db

#endif  // MEM_DB_H_
