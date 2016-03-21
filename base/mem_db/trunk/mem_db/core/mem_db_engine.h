// copyright:
//            (C) SINA Inc.
//
//      file: mem_db_engine.h
//      desc: engine for mem_db
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#ifndef MEM_DB_ENGINE_H_
#define MEM_DB_ENGINE_H_

#include <mem_db/core/mem_db_def_internal.h>

namespace mem_db {

// forward declaration
class MemcachedAgent;

// class MemDBEngine
// In the class, operations stimulate the operation of phyiscal database.
// And different with database, all data is stored in the memory.
// Currently, we use memcached + libmmecached to provide APIs for operations
// all the date is of key-value pair.
// Classical types of date include:
//   1>.Table data item
//  ("table_name&table_key&table_column", "value")
//          to represent detailed data. such as:
//          ("user&100010000&interest", "football");
//   2>. table info in memcached
//   ("&&","value1, value2,...")
//          to represent table list in the mem_db. such as:
//          ("&&","table_user, table_ads"): to represent that there are 2
//          tables in the mem_db: "table_user" and "table_ads", the value
//          list is separated by ',';
//   3>. column info in a table
//   ("table_name&&","column1,column2,...")
//          to represent columns(attributes) the table owns. such as,
//          ("user_info&&","age, uid, interest") to represent there are three attributes
//          in the table "user_info"
//   4>. column attribute for each column in a table
//   ("table_name&&column_name","alive_time,family,bucket_id")
//          to represent descriptors of column column_name in table table_name
//          ("user_info&&interest","1000,2,1") to represent that column interest
//          in table user_info is 1000ms to live, and in family 2, and in bucket
//          1;
class MemDBEngine {
  public:
    MemDBEngine ();
    ~MemDBEngine ();
    // initialization with the configure file
    int Init (const char *conf_file);
    int Free (void);
    int ListHosts (std::vector<HostInfo> &hosts);

    int GetGeneralValue (const std::string &key, std::string &value);
    int MultiGetGeneralValue (const std::vector<std::string> &keys, std::vector<std::string> &values);
    int SetGeneralValue (const std::string &key, const std::string &value, int expiration_time = 0);
    int DeleteGeneralValue (const std::string &key);

    int ListTables (std::vector<std::string> &tables);
    int GetColumns (const std::string &table,
                    std::vector<std::string> &columns);
    int AddColumns (const std::string &table,
                    const std::vector<ColumnDesp> &columns);
    int GetColumnDescriptors (const std::string &table,
                              std::vector<ColumnDesp> &columns);
    int CreateTable (const std::string &table,
                     const std::vector<ColumnDesp> &columns);
    int DeleteTable (const std::string &table);
    int GetTables (const std::vector<MutationTable> &mutations,
                   std::vector<std::vector<std::vector<std::string> > > &table_contents);
    int SetTables (const std::vector<MutationTable> &mutations,
                   const std::vector<std::vector<std::vector<std::string> > > &table_contents);
    int GetRow (const std::string &table,
                const std::string &key,
                std::map<std::string, std::string> &row_record);
    int GetRows (const std::string &table,
                 const std::vector<std::string> &keys,
                 std::vector<std::map<std::string, std::string> > &row_records);
    int PutRow (const std::string &table,
                const std::string &key,
                const std::map<std::string, std::string> &row_record);
    int PutRows (const std::string &table,
                 const std::vector<std::string> &keys,
                 const std::vector<std::map<std::string, std::string> > &row_records);
    int DeleteRow (const std::string &table,
                   const std::string &key);
    int GetRowWithColumns (const std::string &table,
                           const std::string &key,
                           const std::vector<std::string> &columns,
                           std::vector<std::string> &values);
    int GetRowsWithColumns (const std::string &table,
                            const std::vector<std::string> &keys,
                            const std::vector<std::string> &columns,
                            std::vector<std::vector<std::string> > &values);
    int PutRowWithColumns (const std::string &table,
                           const std::string &key,
                           const std::vector<std::string> &columns,
                           const std::vector<std::string> &values);
    int PutRowsWithColumns (const std::string &table,
                            const std::vector<std::string> &keys,
                            const std::vector<std::string> &columns,
                            const std::vector<std::vector<std::string> > &values);
    int DeleteRowWithColumns (const std::string &table,
                              const std::string &key,
                              const std::vector<std::string> &columns);
    int DeleteRowsWithColumns (const std::string &table,
                               const std::vector<std::string> &keys,
                               const std::vector<std::string> &columns);

    int GetColumnExpiration (const std::string &table,
                             const std::string &column,
                             int &expiration);
    int GetColumnExpiration (const std::string &table,
                             const std::vector<std::string> &columns,
                             std::vector<int> &expirations);
    MemcachedAgent *memcached_agent () {
      return memcached_agent_;
    }

  private:
    // handler for libmemcached agent
    MemcachedAgent *memcached_agent_;
    // flags for libmemcached
    FlagTokens memcached_agent_flags_;
    // server vector
    std::vector<HostInfo> server_vector_;

    std::map<std::string, int> table_expiration_;
    int default_expiration_;
    int default_expiration_for_general_kv_;
    // no copy and assignment
    DISALLOW_COPY_AND_ASSIGN (MemDBEngine);
};

}  // namespace mem_db

#endif  // MEM_DB_H_
