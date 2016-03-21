
#include <sstream>


static std::string GenerateNextKey(const std::string key) {

  static uint32_t i = 0;


  std::stringstream ss;
  ss << key << i++;

  return ss.str();
}
