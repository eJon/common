/**
 * @desc: 查询串类，存储查询串相关信息
 * @auth: zibin 
 * @mail: zibin@staff.sina.com.cn 
 * @date: 2011-09-13
 */


#ifndef QUERY_H
#define QUERY_H

#include <string>

/**
 * 原始输入query参数里需要至少如下信息:
 * +----------+-------------------+----------------+
 * |   名称   |        描述       | 备注           |
 * +----------+-------------------+----------------+
 * | uid      | 登陆的微博UID     |                |
 * | ruid     | 被浏览的微博UID   |                |
 * | psid     | 广告位ID          |                |
 * | ipc      | 浏览者IPCODE      |                |
 * | ref      | 浏览页面Url       |                |
 * | cnt      | 广告要求个数      |                |
 * +----------+-------------------+----------------+
 */

/**
 * @desc: 查询串类，存储查询串相关信息
 */
class CQuery
{
public:
	std::string	m_query;			//原始query串
    std::string m_uid;
    std::string m_ruid;
    std::string m_psid;
    std::string m_ref;
    std::string m_ipc;
    std::string m_ver;
	unsigned int m_prodtype;
    int m_cnt;
    std::string	m_queryName;	//请求串中?前的名称

    CQuery()
    {
        m_query = "";
        m_queryName = "";
        m_uid = "";
        m_ruid = "";
        m_psid = "";
        m_ref = "";
        m_ipc = "";
        m_ver = "";
        m_prodtype = 2;
        m_cnt = 1;	
    }
    ~CQuery(){};
};

#endif

