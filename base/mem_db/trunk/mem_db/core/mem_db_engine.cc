// copyright:
//            (C) SINA Inc.
//
//      file: mem_db.h
//      desc: implementation class MemDBEngine
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date:
//
//    change:

#include <mem_db/core/mem_db_engine.h>

#include <mem_db/core/mem_db_conf_def.h>
#include <mem_db/core/mem_db_memcached_agent.h>
#include <mem_db/core/mem_db_util.h>

namespace mem_db {

MemDBEngine::MemDBEngine () : memcached_agent_ (NULL) {
  memset (&memcached_agent_flags_, 0, sizeof (memcached_agent_flags_));

  // clear the server list
  server_vector_.clear ();
  table_expiration_.clear ();
  default_expiration_ = 0;
}

MemDBEngine::~MemDBEngine () {
  this->Free ();
}

int MemDBEngine::Init (const char *conf_file) {
  int ret = 0;
  // load configuration file
  ret = LoadConfigFile (conf_file, server_vector_, memcached_agent_flags_);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  ret = LoadConfigFile (conf_file, table_expiration_, default_expiration_,
                        default_expiration_for_general_kv_);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // initialze memcached agent
  if (NULL != memcached_agent_) {
    delete memcached_agent_;
    memcached_agent_ = NULL;
  }

  memcached_agent_ = new MemcachedAgent ();

  if (NULL == memcached_agent_) {
    return MEM_DB_MEMORY_ERROR;
  }

  ret = memcached_agent_->MemcachedInit (server_vector_, memcached_agent_flags_);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}
int MemDBEngine::Free (void) {
  if (NULL != memcached_agent_) {
    int ret = memcached_agent_->MemcachedFree ();

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    delete memcached_agent_;
    memcached_agent_ = NULL;
  }

  // clear the server list
  server_vector_.clear ();

  return MEM_DB_SUCCESS;
}

int MemDBEngine::ListHosts (std::vector<HostInfo> &hosts) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // return the host vector
  hosts = server_vector_;

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetGeneralValue (const std::string &key, std::string &value) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = memcached_agent_->MemcachedGet (key, value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::MultiGetGeneralValue (const std::vector<std::string> &keys,
                                       std::vector<std::string> &values) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = memcached_agent_->MemcachedGet (keys, values);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::SetGeneralValue (const std::string &key,
                                  const std::string &value,
                                  int expiration_time) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  int expiration = expiration_time;
  if (0 == expiration) {
    expiration = default_expiration_for_general_kv_;
  }

  int ret = memcached_agent_->MemcachedSet (key, value, expiration);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::DeleteGeneralValue (const std::string &key) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  std::string item_key;
  int ret = memcached_agent_->MemcachedDelete (key);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::ListTables (std::vector<std::string> &tables) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  tables.clear ();

  std::string table_info_key;
  std::string table_info_value;

  // generate table info key
  mem_db_util::GenerateTableInfoKey (table_info_key);

  int ret = memcached_agent_->MemcachedGet (table_info_key, table_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // resolve each table name in the table name string
  mem_db_util::ResolveValueString (table_info_value, tables);

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetColumns (const std::string &table,
                             std::vector<std::string> &columns) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  columns.clear ();

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  std::string table_column_info_key;
  std::string table_column_info_value;

  mem_db_util::GenerateTableColumnInfoKey (table, table_column_info_key);

  // get column info
  int ret = memcached_agent_->MemcachedGet (table_column_info_key,
            table_column_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  mem_db_util::ResolveValueString (table_column_info_value, columns);

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetColumnDescriptors (const std::string &table,
                                       std::vector<ColumnDesp> &columns) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  columns.clear ();

  std::vector<std::string> column_names;

  // get columns
  this->GetColumns (table, column_names);

  for (std::vector<std::string>::iterator iter = column_names.begin ();
       iter != column_names.end (); iter++) {
    // get attributes of each column
    // and store the attrs in the output vector
    std::string column_attr_key;
    std::string column_attr_value;

    mem_db_util::GenerateTableColumnAttrKey (table, *iter, column_attr_key);
    int ret = memcached_agent_->MemcachedGet (column_attr_key,
              column_attr_value);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    // resolve the attrs of the column
    std::vector<std::string> attrs;
    mem_db_util::ResolveValueString (column_attr_value, attrs);

    if (attrs.size () != 3) {
      return MEM_DB_INVALID_DATA;
    }

    ColumnDesp descriptor;
    descriptor.name = *iter;
    descriptor.time_to_live = ::atoi (attrs[0].c_str ());
    descriptor.family = ::atoi (attrs[1].c_str ());
    descriptor.bucket_id = ::atoi (attrs[2].c_str ());

    columns.push_back (descriptor);
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::CreateTable (const std::string &table,
                              const std::vector<ColumnDesp> &columns) {
  // the function will update table info, table column info and table column
  // attribute info. However, the 3 operatiops is not atomil operations
  // termporarily. If the function is called failure, some operatoins may still
  // done successfully. The failure to call the function just means at least
  // one operation is done failure, but not each of them.
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;
  std::string table_column_info_key;
  std::string table_column_info_value;
  std::vector<std::string> column_attr_key_vector;
  std::vector<std::string> column_attr_value_vector;

  // 1. Generate keys and values
  mem_db_util::GenerateTableColumnInfoKey (table, table_column_info_key);

  for (std::vector<ColumnDesp>::const_iterator iter = columns.begin ();
       iter != columns.end (); iter++) {
    if (true != mem_db_util::IsAlnumString (iter->name)) {
      return MEM_DB_PARAMETER_ERR;
    }

    std::string column_attr_key;
    mem_db_util::GenerateTableColumnAttrKey (table, iter->name,
        column_attr_key);
    column_attr_key_vector.push_back (column_attr_key);
    std::string column_attr;
    mem_db_util::GenerateColumnAttrValueString (*iter, column_attr);
    column_attr_value_vector.push_back (column_attr);
  }

  // 2. Check whethe the table alread exists or not in the memcached, if not,
  // update table info value
  std::string table_info_key;
  std::string table_info_value;
  mem_db_util::GenerateTableInfoKey (table_info_key);
  ret = memcached_agent_->MemcachedGet (table_info_key, table_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  std::vector<std::string> table_info_value_vector;
  mem_db_util::ResolveValueString (table_info_value,
                                   table_info_value_vector);
  std::vector<std::string>::iterator iter = table_info_value_vector.begin ();

  for (; iter != table_info_value_vector.end (); iter++) {
    if (0 == iter->compare (table)) {
      // already exist
      break;
    }
  }

  if (table_info_value_vector.end () == iter ) {
    // the table does not exist
    table_info_value_vector.push_back (table);
    mem_db_util::GenerateValueString (table_info_value_vector,
                                      table_info_value);
    // overwrite existing table info record
    ret = memcached_agent_->MemcachedSet (table_info_key,
                                          table_info_value);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }  // if the table exists, do not insert the table name

  // 3. Insert table column info. generate table info value from column names,
  // which are the keys of column attr records
  mem_db_util::GenerateValueString (column_attr_key_vector,
                                    table_column_info_value);
  // add colum info into memcached. if there is alreay data there, just
  // overwrite the data record
  ret = memcached_agent_->MemcachedSet (table_column_info_key,
                                        table_column_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // 4. Add colum attrs info into memcached. If there is alreay data there,
  // just overwrite the data record table column attr info
  ret = memcached_agent_->MemcachedSet (column_attr_key_vector,
                                        column_attr_value_vector);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::DeleteTable (const std::string &table) {
  // the function will update table info, table column info and table column
  // attribute info. However, the 3 operatiops is not atomil operations
  // termporarily. If the function is called failure, some operatoins may still
  // done successfully. The failure to call the function just means at least
  // one operation is done failure, but not each of them.

  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;
  std::string table_column_info_key;
  std::string table_column_info_value;
  mem_db_util::GenerateTableColumnInfoKey (table,
      table_column_info_key);
  ret = memcached_agent_->MemcachedGet (table_column_info_key,
                                        table_column_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return MEM_DB_SYSTEM_ERROR;
  }

  std::vector<std::string> column_names;
  mem_db_util::ResolveValueString (table_column_info_value,
                                   column_names);

  std::string table_column_attr_key;
  std::vector<std::string> table_column_attr_key_vector;

  for (std::vector<std::string>::iterator iter = column_names.begin ();
       iter != column_names.end (); iter++) {
    // generate column attr keys
    std::string table_column_attr_key;
    mem_db_util::GenerateTableColumnAttrKey (table, *iter,
        table_column_attr_key);
    table_column_attr_key_vector.push_back (table_column_attr_key);
  }

  // delete column attribute records that get involved with the table in
  // memcached
  ret = memcached_agent_->MemcachedDelete (table_column_attr_key_vector);

  if (MEM_DB_SUCCESS != ret) {
    return MEM_DB_SYSTEM_ERROR;
  }

  // delete table record in memcached
  ret = memcached_agent_->MemcachedDelete (table_column_info_key);

  if (MEM_DB_SUCCESS != ret) {
    return MEM_DB_SYSTEM_ERROR;
  }

  // delete table info in memcached
  std::string table_info_key;
  std::string table_info_value;
  std::vector<std::string> table_info_value_vector;

  mem_db_util::GenerateTableInfoKey (table_info_key);
  ret = memcached_agent_->MemcachedGet (table_info_key, table_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  mem_db_util::ResolveValueString (table_info_value,
                                   table_info_value_vector);
  std::vector<std::string>::iterator iter = table_info_value_vector.begin ();

  for (; iter != table_info_value_vector.end (); iter++) {
    if (0 == iter->compare (table)) {
      // already exist
      break;
    }
  }

  if (table_info_value_vector.end () != iter ) {
    // delete the table name in the table info value list
    table_info_value_vector.erase (iter);
    mem_db_util::GenerateValueString (table_info_value_vector,
                                      table_info_value);
    // overwrite existing table info record
    // update table info value in memcached
    ret = memcached_agent_->MemcachedSet (table_info_key,
                                          table_info_value);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }  // if the table exists, do not insert the table name

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetTables (
  const std::vector<MutationTable> &mutations,
  std::vector<std::vector<std::vector<std::string> > > &table_contents) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  table_contents.clear ();

  int ret = 0;
#if 0

  for (std::vector<MutationTable>::const_iterator iter = mutations.begin ();
       iter != mutations.end (); iter++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (iter->table_name)) {
      return MEM_DB_PARAMETER_ERR;
    }

    std::vector<std::vector<std::string> > table_content;
    ret = this->GetRowsWithColumns (iter->table_name,
                                    iter->row_keys,
                                    iter->column_specifiers,
                                    table_content);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    table_contents.push_back (table_content);
  }

#else

  size_t len_table = mutations.size ();
  size_t len_row = 0;
  size_t len_column = 0;
  std::vector<size_t> mutation_lengths;
  std::vector<std::string> mem_keys;
  std::string mem_key;

  mutation_lengths.clear ();

  for (size_t i = 0; i < len_table; ++i) {
    if (true != mem_db_util::IsAlnumString (mutations[i].table_name)) {
      return MEM_DB_PARAMETER_ERR;
    }

    len_row = mutations[i].row_keys.size ();
    len_column = mutations[i].column_specifiers.size ();
    mutation_lengths.push_back (len_row);
    mutation_lengths.push_back (len_column);

    for (size_t j = 0; j < len_row; ++j) {
      if (true != mem_db_util::IsAlnumString (mutations[i].row_keys[j])) {
        return MEM_DB_PARAMETER_ERR;
      }

      for (size_t k = 0; k < len_column; ++k) {
        if (true != mem_db_util::IsAlnumString (mutations[i].column_specifiers[k])) {
          return MEM_DB_PARAMETER_ERR;
        }

        mem_db_util::GenerateTableDataItemKey (mutations[i].table_name,
                                               mutations[i].row_keys[j],
                                               mutations[i].column_specifiers[k],
                                               mem_key);
        mem_keys.push_back (mem_key);
      }
    }
  }

  std::vector<std::string> values;
  ret = memcached_agent_->MemcachedGet (mem_keys, values);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  size_t index = 0;

  for (size_t i = 0; i < len_table; ++i) {
    len_row = mutation_lengths[i * 2];
    len_column = mutation_lengths[i * 2 + 1];
    std::vector<std::vector<std::string> > values_by_rows;

    for (size_t j = 0; j < len_row; ++j) {
      std::vector<std::string> values_by_columns;
      values_by_columns.clear ();

      for ( size_t k = 0; k < len_column; ++k) {
        values_by_columns.push_back (values[index++]);
      }

      //values.erase(values.begin(), values.begin() + len_column - 1);
      values_by_rows.push_back (values_by_columns);
    }

    table_contents.push_back (values_by_rows);
  }

  if (index != values.size ()) {
    //printf("%d---------%d", int(index), int(values.size()));
    return MEM_DB_SYSTEM_ERROR;
  }

#endif
  return MEM_DB_SUCCESS;
}

int MemDBEngine::SetTables (const std::vector<MutationTable> &mutations,
                            const std::vector<std::vector<std::vector<std::string> > > &table_contents) {
  // the function just update data items, not table infos
  // if you want to update table info in memcached, call corresponding APIs to
  // do it.
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;
  size_t len_tables = mutations.size ();
  size_t len_contents = table_contents.size ();

  if (len_tables != len_contents) {
    return MEM_DB_PARAMETER_ERR;
  }

  for (size_t i = 0; i < len_tables; ++i) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (mutations[i].table_name)) {
      return MEM_DB_PARAMETER_ERR;
    }

    ret = this->PutRowsWithColumns (mutations[i].table_name,
                                    mutations[i].row_keys,
                                    mutations[i].column_specifiers,
                                    table_contents[i]);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::AddColumns (const std::string &table,
                             const std::vector<ColumnDesp> &columns) {
  // The function will update table info, table column info and table column
  // attribute info. However, the 3 operatiops is not atomil operations
  // termporarily If the function is called failure, some operatoins may still
  // done successfully. The failure to call the function just means at least
  // one operation is done failure, but not each of them. If there is already
  // (or part of)data, just overwrite existing data in memcached
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  return this->CreateTable (table, columns);
}

int MemDBEngine::GetRowWithColumns (const std::string &table,
                                    const std::string &key,
                                    const std::vector<std::string> &columns,
                                    std::vector<std::string> &values) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  values.clear ();

  // check parameters
  if ((true != mem_db_util::IsAlnumString (table))
      || (true != mem_db_util::IsAlnumString (key))) {
    return MEM_DB_PARAMETER_ERR;
  }

  std::vector<std::string> table_data_item_info_key_vector;
  std::string table_data_item_info_key;

  for (std::vector<std::string>::const_iterator iter = columns.begin ();
       iter != columns.end (); iter++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (*iter)) {
      return MEM_DB_PARAMETER_ERR;
    }

    mem_db_util::GenerateTableDataItemKey (table, key, *iter,
                                           table_data_item_info_key);
    table_data_item_info_key_vector.push_back (table_data_item_info_key);
  }

  return memcached_agent_->MemcachedGet (table_data_item_info_key_vector,
                                         values);
}

int MemDBEngine::GetRowsWithColumns (const std::string &table,
                                     const std::vector<std::string> &keys,
                                     const std::vector<std::string> &columns,
                                     std::vector<std::vector<std::string> > &values) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  values.clear ();

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  int columns_size = (int)columns.size ();

  for (int i = 0; i < columns_size; i++) {
    if (true != mem_db_util::IsAlnumString (columns[i])) {
      return MEM_DB_PARAMETER_ERR;
    }
  }

  std::vector<std::string> table_data_item_info_key_vector;
  int keys_size = (int)keys.size ();

  for (int i = 0; i < keys_size; i++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (keys[i])) {
      return MEM_DB_PARAMETER_ERR;
    }

    for (int j = 0; j < columns_size; j++) {
      std::string table_data_item_info_key;
      mem_db_util::GenerateTableDataItemKey (table, keys[i], columns[j],
                                             table_data_item_info_key);
      table_data_item_info_key_vector.push_back (table_data_item_info_key);
    }
  }

  std::vector<std::string> all_items;
  int ret = memcached_agent_->MemcachedGet (table_data_item_info_key_vector,
            all_items);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  int all_items_size = (int)all_items.size ();

  if (all_items_size != (keys_size * columns_size)) {
    return MEM_DB_SYSTEM_ERROR;
  }

  int pos = 0;

  for (int i = 0; i < keys_size; i++) {
    std::vector<std::string> key_items;

    for (int j = 0; j < columns_size; j++) {
      key_items.push_back (all_items[pos++]);
    }

    values.push_back (key_items);
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetRow (const std::string &table,
                         const std::string &key,
                         std::map<std::string, std::string> &row_record) {
  // The fucntion has to retrieve table info from the memcached in order to
  // fetch data. The consistence beween table info and table data item should
  // be maintained externally. mem_db does NOT support consistence between
  // data and table info termporarily
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  row_record.clear ();

  // table --> columns info
  int ret = 0;
  std::string table_column_info_key;
  std::string table_column_info_value;
  std::vector<std::string> table_column_info_value_vector;

  mem_db_util::GenerateTableColumnInfoKey (table, table_column_info_key);
  ret = memcached_agent_->MemcachedGet (table_column_info_key, table_column_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // check whethe the table alread exists or not in the memcached
  // (table, key, column) --> data items
  mem_db_util::ResolveValueString (table_column_info_value,
                                   table_column_info_value_vector);
  std::vector<std::string> values;
  ret = this->GetRowWithColumns (table, key, table_column_info_value_vector,
                                 values);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  size_t len_values = values.size ();
  size_t len_column_info_value = table_column_info_value_vector.size ();

  if (len_column_info_value != len_values) {
    return MEM_DB_INVALID_DATA;
  }

  for (size_t i = 0; i < len_values; ++i) {
    row_record.insert (std::pair<std::string, std::string> (
                         table_column_info_value_vector[i], values[i]));
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetRows (const std::string &table,
                          const std::vector<std::string> &keys,
                          std::vector<std::map<std::string, std::string> > &row_records) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  row_records.clear ();

  int ret = 0;

  for (std::vector<std::string>::const_iterator iter = keys.begin ();
       iter != keys.end (); iter++) {
    std::map<std::string, std::string> row_record;
    ret = this->GetRow (table, *iter, row_record);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    row_records.push_back (row_record);
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::PutRowWithColumns (const std::string &table,
                                    const std::string &key,
                                    const std::vector<std::string> &columns,
                                    const std::vector<std::string> &values) {
  // the function just update data items, not table infos. If you want to
  // update table info in memcached, call corresponding APIs to do it.
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if ((true != mem_db_util::IsAlnumString (table))
      || (true != mem_db_util::IsAlnumString (key))) {
    return MEM_DB_PARAMETER_ERR;
  }

  int len_columns = columns.size ();
  int len_values = values.size ();

  if (len_columns != len_values) {
    return MEM_DB_PARAMETER_ERR;
  }

  std::string table_data_item_info_key;
  std::vector<std::string> table_data_item_info_key_vector;

  for (std::vector<std::string>::const_iterator iter = columns.begin ();
       iter != columns.end (); iter++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (*iter)) {
      return MEM_DB_PARAMETER_ERR;
    }

    mem_db_util::GenerateTableDataItemKey (table, key, *iter,
                                           table_data_item_info_key);
    table_data_item_info_key_vector.push_back (table_data_item_info_key);
  }

  int expiration = default_expiration_;
  std::map<std::string, int>::iterator liter
    = table_expiration_.find (table);

  if (liter != table_expiration_.end ()) {
    expiration = liter->second;
  }

  return memcached_agent_->MemcachedSet (table_data_item_info_key_vector,
                                         values, expiration);
}

int MemDBEngine::PutRowsWithColumns (
  const std::string &table,
  const std::vector<std::string> &keys,
  const std::vector<std::string> &columns,
  const std::vector<std::vector<std::string> > &values) {
  // the function just update data items, not table infos. If you want to
  // update table info in memcached, call corresponding APIs to do it.
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;
  size_t len_keys = keys.size ();
  size_t len_values = values.size ();

  if (len_keys != len_values) {
    return MEM_DB_PARAMETER_ERR;
  }

  for (size_t i = 0; i < len_keys; ++i) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (keys[i])) {
      return MEM_DB_PARAMETER_ERR;
    }

    ret = this->PutRowWithColumns (table, keys[i], columns, values[i]);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::PutRow (const std::string &table,
                         const std::string &key,
                         const std::map<std::string, std::string> &row_record) {
  // the function just update data items, not table infos. If you want to
  // update table info in memcached, call corresponding APIs to do it.
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  std::vector<std::string> columns;
  std::vector<std::string> values;

  for (std::map<std::string, std::string>::const_iterator iter = row_record.begin ();
       iter != row_record.end (); iter++) {
    columns.push_back (iter->first);
    values.push_back (iter->second);
  }

  return this->PutRowWithColumns (table, key, columns, values);
}

int MemDBEngine::PutRows (const std::string &table,
                          const std::vector<std::string> &keys,
                          const std::vector<std::map<std::string, std::string> > &row_records) {
  // the function just update data items, not table infos. If you want to
  // update table info in memcached, call corresponding APIs to do it.

  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  size_t len_keys = keys.size ();
  size_t len_rows = row_records.size ();

  if (len_keys != len_rows) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;

  for (size_t i = 0; i < len_keys; ++i) {
    ret = this->PutRow (table, keys[i], row_records[i]);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::DeleteRow (const std::string &table, const std::string &key) {
  // the function just delete data items. If table is empty after deleting the
  // data items, the table info will never be removed, instead, the table info
  // is still available in memcached
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  // get columns
  int ret = 0;
  std::vector<std::string> columns;
  ret = this->GetColumns (table, columns);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  // call deleteRowWithColumns()
  return this->DeleteRowWithColumns (table, key, columns);
}

int MemDBEngine::DeleteRowWithColumns (
  const std::string &table,
  const std::string &key,
  const std::vector<std::string> &columns) {
  // the function just delete data items. If table is empty after deleting the
  // data items, the table info will never be removed, instead, the table info
  // is still available in memcached
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if ((true != mem_db_util::IsAlnumString (table))
      || (true != mem_db_util::IsAlnumString (key))) {
    return MEM_DB_PARAMETER_ERR;
  }

  std::vector<std::string> table_data_item_info_key_vector;
  std::string table_data_item_info_key;

  for (std::vector<std::string>::const_iterator iter = columns.begin ();
       iter != columns.end (); iter++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (*iter)) {
      return MEM_DB_PARAMETER_ERR;
    }

    mem_db_util::GenerateTableDataItemKey (table, key, *iter,
                                           table_data_item_info_key);
    table_data_item_info_key_vector.push_back (table_data_item_info_key);
  }

  return memcached_agent_->MemcachedDelete (table_data_item_info_key_vector);
}

int MemDBEngine::DeleteRowsWithColumns (const std::string &table,
                                        const std::vector<std::string> &keys,
                                        const std::vector<std::string> &columns) {
  // the function just delete data items. If table is empty after deleting the
  // data items, the table info will never be removed, instead, the table info
  // is still available in memcached
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if (true != mem_db_util::IsAlnumString (table)) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;

  for (std::vector<std::string>::const_iterator iter = keys.begin ();
       iter != keys.end (); iter++) {
    // check parameters
    if (true != mem_db_util::IsAlnumString (*iter)) {
      return MEM_DB_PARAMETER_ERR;
    }

    ret = this->DeleteRowWithColumns (table, *iter, columns);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }
  }

  return MEM_DB_SUCCESS;
}

int MemDBEngine::GetColumnExpiration (const std::string &table,
                                      const std::string &column,
                                      int &expiration) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  // check parameters
  if ((true != mem_db_util::IsAlnumString (table))
      || (true != mem_db_util::IsAlnumString (column))) {
    return MEM_DB_PARAMETER_ERR;
  }

  int ret = 0;
  std::string column_attr_info_key;
  std::string column_attr_info_value;
  mem_db_util::GenerateTableColumnAttrKey (table, column, column_attr_info_key);

  ret = memcached_agent_->MemcachedGet (column_attr_info_key,
                                        column_attr_info_value);

  if (MEM_DB_SUCCESS != ret) {
    return ret;
  }

  if (0 == column_attr_info_value.compare ("")) {
    // not existed
    expiration = 0;
    return MEM_DB_SUCCESS;
  }

  std::vector<std::string> attrs;
  mem_db_util::ResolveValueString (column_attr_info_value, attrs);

  if (attrs.size () != 3) {
    return MEM_DB_INVALID_DATA;
  }

  expiration = ::atoi (attrs[0].c_str ());

  return MEM_DB_SUCCESS;
}
int MemDBEngine::GetColumnExpiration (const std::string &table,
                                      const std::vector<std::string> &columns,
                                      std::vector<int> &expirations) {
  if (NULL == memcached_agent_) {
    return MEM_DB_INVALID_DATA;
  }

  int ret = 0;
  int expiration;

  for (std::vector<std::string>::const_iterator iter = columns.begin ();
       iter != columns.end (); iter++) {
    ret = GetColumnExpiration (table, *iter, expiration);

    if (MEM_DB_SUCCESS != ret) {
      return ret;
    }

    expirations.push_back (expiration);
  }

  return MEM_DB_SUCCESS;
}

}  // namespace
