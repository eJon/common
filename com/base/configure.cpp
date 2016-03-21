#include <matchserver/mscom/base/configure.h>

using namespace std;
namespace ms {
namespace common{

Configure::Configure() {
    // TODO Auto-generated constructor stub
}

Configure::~Configure() {
    // TODO Auto-generated destructor stub
}
int Configure::Initialize(const char *config_file_name) {
    if(config_file_name == NULL) {
        ERROR(LOGROOT, "[configure]configure file name is NULL");
        return CONFIGURE_ERR;
    }

    struct stat file_stat;

    if(0 != stat(config_file_name, &file_stat)) {
        //file not exist
        ERROR(LOGROOT, "[configure]configure file is not exist(%s)", config_file_name);
        return CONFIGURE_ERR;
    }

    base_file_name_ = config_file_name;
    int pos = base_file_name_.rfind('/');
    base_file_name_ = (pos == -1) ? base_file_name_ : base_file_name_.substr(0, pos + 1);
    //call base class
    load(config_file_name);
    return CONFIGURE_OK;
}
int Configure::ReloadConfigure(const char *config_file_name) {
    return Initialize(config_file_name);
}
std::string  Configure::GetBasePath() {
    return base_file_name_;
}
} /* namespace ms */

}
