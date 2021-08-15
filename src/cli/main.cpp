#include <iostream>

#include "bulbs_discover.h"

void print_info(BulbInfo &info) {
  for (auto &pair : info) {
    std::cout << "prop=" << pair.first << " val=" << pair.second << std::endl;
  }
}

int main() {
  auto res = BulbsDiscover::discover_by_ssdp(std::chrono::seconds(3));
  std::cout << "discovered " << res.size() << " bulbs" << std::endl;

  for (auto &pair : res) {
    print_info(pair.second);
  }
}