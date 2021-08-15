#include <chrono>
#include <map>
#include <string>

using BulbInfo = std::map<std::string, std::string>;
using DiscoverResult = std::map<std::string, BulbInfo>;

class BulbsDiscover {
private:
  static BulbInfo parse_ssdp_response(std::string ssdp_response);

public:
  static DiscoverResult discover_by_ssdp(std::chrono::duration<int> timeout);
};
