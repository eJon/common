/*
 * ms_configure.h
 *
 *  Created on: 2013-3-15
 *      Author: andrew
 */

#ifndef MS_CONFIGURE_H_
#define MS_CONFIGURE_H_


#include <sys/stat.h>
#include <string>
#include <mscom/base/INIParser.h>
#include <matchserver/mscom/base/define_const.h>
#include <matchserver/mscom/base/log.h>

using namespace utility;
namespace ms {
namespace common{
class Configure: public utility::INIParser {
public:
    Configure();
    virtual ~Configure();
public:
    /*
    **bref  initialize from configure file
    ** param[in] config_file_name
    ** return,0-OK
    */
    int Initialize(const char *config_file_name);
    int ReloadConfigure(const char *config_file_name);
    std::string GetBasePath();
    static std::string GetAbsolutePath(const std::string &base, const std::string &relativePath) {
        return base + relativePath;
    }

#ifdef UTEST_MSCONFIGURE
public:
#else
private:
#endif
    std::string base_file_name_;

};

} /* namespace ms */
}
#endif /* MS_CONFIGURE_H_ */

