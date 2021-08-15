#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <sys/socket.h>
#include <vector>

#include "bulbs_discover.h"

static const uint16_t ssdp_port = 1982;

static const std::string sspd_message = "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1982\r\nMAN: "
                                        "\"ssdp:discover\"\r\nST: wifi_bulb";

static const sockaddr_in ssdp_target_addr = {.sin_family = AF_INET,
                                             .sin_port = htons(ssdp_port),
                                             .sin_addr.s_addr = inet_addr("239.255.255.250")};
static const struct sockaddr_in listen_addr = {
    .sin_family = AF_INET, .sin_port = htons(ssdp_port), .sin_addr.s_addr = INADDR_ANY};

static std::tuple<std::string, std::string> split_str(const std::string &s, char delim) {
  std::tuple<std::string, std::string> result;

  int pos = s.find(':');
  std::string first = s.substr(0, pos);

  if (pos + 2 >= s.length())
    throw std::invalid_argument("invalid header");

  std::string sec = s.substr(pos + 2);
  return std::tuple(first, sec);
}

BulbInfo BulbsDiscover::parse_ssdp_response(std::string ssdp_response) {
  BulbInfo result;
  std::string line;
  std::stringstream ss(ssdp_response);

  std::getline(ss, line, '\n');
  if (line != "HTTP/1.1 200 OK\r") {
    throw std::logic_error("non successful ssdp response");
  }

  while (std::getline(ss, line, '\n')) {
    result.insert(split_str(line, ':'));
  }

  return result;
}

DiscoverResult BulbsDiscover::discover_by_ssdp(std::chrono::duration<int> timeout) {
  DiscoverResult bulbs;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (bind(sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
    throw std::logic_error("could not bind socket");
  }

  ssize_t rc = sendto(sock, sspd_message.c_str(), sspd_message.length(), 0,
                      (struct sockaddr *)&ssdp_target_addr, sizeof(ssdp_target_addr));

  if (rc < 0)
    throw std::logic_error("could not send ssdp discovery packet");

  // Set recieve timeout.
  struct timeval tv = {.tv_sec = timeout.count(), .tv_usec = 0};
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    throw std::logic_error("setsockopt failed");

  while (true) {
    char buffer[1024];
    struct sockaddr from;
    socklen_t sizefrom = sizeof(from);

    rc = recvfrom(sock, &buffer, 1024, 0, &from, &sizefrom);
    if (rc == -1) {
      // timeout occured, return discovered bulbs.
      if (errno == EAGAIN)
        return bulbs;

      throw std::logic_error("recvfrom failed");
    }

    std::string ssdp_response(buffer, rc);

    auto bulb_info = parse_ssdp_response(ssdp_response);
    bulbs.insert_or_assign(bulb_info["Location"], bulb_info);
  }
}