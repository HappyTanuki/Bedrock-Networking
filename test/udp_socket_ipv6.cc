#include <cstdlib>
#include <future>
#include <iostream>
#include <string>
#include <thread>

#include "networking/address.h"
#include "networking/socket.h"
#include "networking/wsa.h"

int ServerProcess(bedrock::network::Socket sock);
int ClientProcess(std::uint16_t port);

int main() {
  bedrock::network::WSAManager wsamanager;

  bedrock::network::Address addr;
  addr.SetAddr(bedrock::network::IPVersion::kIPV6, "::", 0);

  bedrock::network::Socket sock(bedrock::network::SocketType::kUDP, addr);
  if (sock.Init() != bedrock::network::SocketErrorStatus::kSuccess) {
    std::cout << "[Server]: Error: " << sock.GetErrorMessage() << std::endl;
    return EXIT_FAILURE;
  }

  std::uint16_t port = 0;
  while (sock.Bind() != bedrock::network::SocketErrorStatus::kSuccess) {
    addr.SetAddr(bedrock::network::IPVersion::kIPV6, "::", ++port);
  }
  port = sock.GetAddr().data.GetPort().data;

  std::packaged_task<int(bedrock::network::Socket)> server_process_task(
      [](bedrock::network::Socket&& _socket) {
        return ServerProcess(std::move(_socket));
      });
  std::packaged_task<int(std::uint16_t)> client_process_task(
      [](std::uint16_t _port) { return ClientProcess(_port); });

  std::future<int> server_process_task_return_future =
      server_process_task.get_future();
  std::future<int> client_process_task_return_future =
      client_process_task.get_future();

  std::thread server(std::move(server_process_task), std::move(sock));
  std::thread client(std::move(client_process_task), port);

  server.join();
  client.join();

  if (server_process_task_return_future.get() != EXIT_SUCCESS ||
      client_process_task_return_future.get() != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int ServerProcess(bedrock::network::Socket sock) {
  bedrock::network::Address addr = sock.GetAddr().data;

  std::cout << "[Server]: Server is now listening on: "
            << static_cast<std::string>(addr) << std::endl;

  auto ret_read = sock.Read(BUFSIZ);

  if (ret_read.status == bedrock::network::SocketErrorStatus::kDisconnect) {
    std::cout << "[Server]: Info: Peer disconnected" << std::endl;
    return EXIT_SUCCESS;
  } else if (ret_read.status != bedrock::network::SocketErrorStatus::kSuccess) {
    std::cout << "[Server]: Error: " << sock.GetErrorMessage() << std::endl;
    return EXIT_FAILURE;
  }

  if (ret_read.data.first.size() != ret_read.data.second) {
    ret_read.data.first.resize(ret_read.data.second);
  }

  sock.Write(ret_read.data.first);

  std::cout << "[Server]: Echoed \""
            << std::string(
                   reinterpret_cast<const char*>(ret_read.data.first.data()),
                   ret_read.data.first.size())
            << "\""
            << " to :" << static_cast<std::string>(sock.GetAddr().data)
            << std::endl;

  return EXIT_SUCCESS;
}

int ClientProcess(std::uint16_t port) {
#ifdef _WIN32
  Sleep(1000);
#else
  ::sleep(1);
#endif

  bedrock::network::Address addr;
  addr.SetAddr(bedrock::network::IPVersion::kIPV6, "::1", port);

  bedrock::network::Socket sock(bedrock::network::SocketType::kUDP, addr);
  if (sock.Init() != bedrock::network::SocketErrorStatus::kSuccess) {
    std::cout << "[Client]: Error: " << sock.GetErrorMessage() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "[Client]: Client is now talking to: "
            << static_cast<std::string>(addr) << std::endl;
  std::string input = "Test string.";

  auto bytes = std::as_bytes(std::span(input));

  sock.Write(bytes);

  std::cout << "[Client]: Sent: " << input << std::endl;

  auto read = sock.Read(BUFSIZ);
  if (read.status != bedrock::network::SocketErrorStatus::kSuccess) {
    std::cout << "[Client]: Error: " << sock.GetErrorMessage() << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "[Client]: Received: \""
            << std::string(
                   reinterpret_cast<const char*>(read.data.first.data()),
                   read.data.first.size())
            << "\" from " << static_cast<std::string>(addr) << std::endl;

  return EXIT_SUCCESS;
}
