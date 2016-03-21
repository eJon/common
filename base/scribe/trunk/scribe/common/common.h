#ifndef SCRIBESERVER_COMMON_H_
#define SCRIBESERVER_COMMON_H_

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <tr1/memory>

#define SCRIBESERVER_BEGIN_NAMESPACE_SCRIBE_SERVER namespace scribe_server {

#define SCRIBESERVER_END_NAMESPACE_SCRIBE_SERVER }

#define SCRIBESERVER_USE_NAMESPACE_SCRIBE_SERVER using namespace scribe_server;

typedef std::map<std::string, std::string> StrRbMap;

#endif //SCRIBESERVER_COMMON_H_
