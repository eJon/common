#ifndef _IPLUGIN_H_
#define _IPLUGIN_H_

class iplugin_t {
  public:
    virtual ~iplugin_t() {};
  public:
    //插件初始化
    virtual int initialize(const char *configure_file) = 0;
    //插件反初始化
    virtual int uninitialize() = 0;
    //重新加载配置, 如果使用configure_file = NULL,使用initialize时传入的initialize
    virtual int reload_cfg(const char *configure_file = NULL) = 0;
    //获取统计信息
    virtual int statistics(char *buffer, size_t &len) = 0;
};

#endif
