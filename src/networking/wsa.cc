#include "networking/wsa.h"

namespace bedrock::network {

WSAManager::WSAManager() {
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    valid = false;
  } else {
    valid = true;
  }
#endif
}

WSAManager::~WSAManager() {
#ifdef _WIN32
  if (valid) {
    WSACleanup();
  }
#endif
}

}  // namespace bedrock::network
