#pragma once
#ifndef _BS_LIB_DUMP_H
#define _BS_LIB_DUMP_H

#include <gflags/gflags.h>
#include <fstream>          // std::ostream
#include "das_lib_log.h"

namespace das_lib{

template<class _AttrS,class _Tup,class _SerializerType> struct TupleDumpHelper;

template<class _Head,class _Tail,class _Tup,class _SerializerType>
struct TupleDumpHelper< Cons<_Head, _Tail>,_Tup,_SerializerType>
{
    static int dump_tuple(const _Tup* tuple,_SerializerType& sz)
    {
        if (NULL == tuple) {
            DL_LOG_WARNING("dump tuple is NULL");
            return -1;
        }

        sz <<  _Head::name() << "{";
        typename _Head::Type val = tuple->template at<_Head>();
        sz << val;
        sz << "}\t";
        
        return TupleDumpHelper<_Tail,_Tup,_SerializerType>::dump_tuple(tuple,sz);
    }
};

template<class _Tup,class _SerializerType>
struct TupleDumpHelper< void,_Tup,_SerializerType>
{
    static int dump_tuple(const _Tup* ,_SerializerType& )
    {
        return 0;
    }
};
    
/*@brief:
 *    this function is used for serializing small table 
 *    before using this class, make sure that char* type and each column type in small table can 
 *       be serialized by sz of the type SerializerType
 *    say:
 *    if we define ST as: 
 *    DEFINE_ATTRIBUTE (USER_ID, uint32_t);
 *    DEFINE_ATTRIBUTE (USER_STAT, user_stat_t);
 *    typedef ST_TABLE(USER_ID, USER_STAT, ST_UNIQUE_KEY(USER_ID)) UserStatTable;
 *    when dumping UserStatTable, make sure (1) uint32_t type(of course it does in most SerializerType) and 
 *       (2) user_stat_t(which u may forget) can be serialized by sz of SerializerType
 *       (3) char* can be serialized by sz of SerializerType
 *@param: table [in] table to be serialized;
 *@param: sz    [in] serializer
 *@param: decs  [in] description
 */

template<
class TableType,
class SerializerType    
>    
int dump_table(const TableType *table,SerializerType& sz,const char* desc)
{
    if(NULL == table) {
        DL_LOG_FATAL("dump table failed due to NULL table pointer");
        return -1;
    }

    if(NULL == desc) {
        DL_LOG_FATAL("dump table failed due to NULL desc string");
        return -1;
    }


    typename TableType::Iterator it  = table->begin();
    typename TableType::Iterator ite = table->end();

    int ret = -1;
    for(;it != ite; ++it) {
        sz << desc << ":";
        const typename TableType::Tup& tuple = *it;
        ret = TupleDumpHelper<typename TableType::AttrS, typename TableType::Tup,SerializerType>::dump_tuple(&tuple ,sz);
        if(ret < 0) {
            DL_LOG_FATAL("dumper table tuple failed");
            return -1;
        }
        sz << "\n";
    }

    return 0;
}
}  // namespace das_lib

#endif  // BS_LIB_DUMPER_H

