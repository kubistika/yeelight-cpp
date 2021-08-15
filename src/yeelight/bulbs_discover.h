#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <string_view>
#include <sys/socket.h>
#include <vector>

using BulbInfo = std::map<std::string, std::string>;

class BulbsDiscover {
private:
  static std::tuple<std::string, std::string> split(const std::string &s, char delim);
  static BulbInfo parse_ssdp_response(std::string ssdp_response);

public:
  static std::map<std::string, BulbInfo> discover(std::chrono::duration<int> timeout);
};
