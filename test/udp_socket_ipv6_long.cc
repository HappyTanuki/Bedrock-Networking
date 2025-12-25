#include <cstdlib>
#include <future>
#include <iostream>
#include <mutex>
#include <span>
#include <string>
#include <thread>

#include "networking/address.h"
#include "networking/socket.h"
#include "networking/wsa.h"

static std::mutex cout_mutex;

int ServerProcess(bedrock::network::Socket sock);
int ClientProcess(std::uint16_t port);

int main() {
  bedrock::network::WSAManager wsamanager;

  bedrock::network::Address addr;
  addr.SetAddr(bedrock::network::IPVersion::kIPV6, "::", 0);

  bedrock::network::Socket sock(bedrock::network::SocketType::kUDP, addr);
  if (sock.Init() != bedrock::network::SocketErrorStatus::kSuccess) {
    cout_mutex.lock();
    std::cout << "[Server]: Error: " << sock.GetErrorMessage() << std::endl;
    cout_mutex.unlock();
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

  cout_mutex.lock();
  std::cout << "[Server]: Server is now listening on: "
            << static_cast<std::string>(addr) << std::endl;
  cout_mutex.unlock();

  auto ret_read = sock.Read(BUFSIZ);

  if (ret_read.status == bedrock::network::SocketErrorStatus::kDisconnect) {
    cout_mutex.lock();
    std::cout << "[Server]: Info: Peer disconnected" << std::endl;
    cout_mutex.unlock();
    return EXIT_SUCCESS;
  } else if (ret_read.status != bedrock::network::SocketErrorStatus::kSuccess) {
    cout_mutex.lock();
    std::cout << "[Server]: Error: " << sock.GetErrorMessage() << std::endl;
    cout_mutex.unlock();
    return EXIT_FAILURE;
  }

  if (ret_read.data.first.size() != ret_read.data.second) {
    ret_read.data.first.resize(ret_read.data.second);
  }

  sock.Write(ret_read.data.first);

  cout_mutex.lock();
  std::cout << "[Server]: Echoed " << ret_read.data.first.size() << "bytes "
            << "to :" << static_cast<std::string>(sock.GetAddr().data)
            << std::endl;
  cout_mutex.unlock();

  return EXIT_SUCCESS;
}

int ClientProcess(std::uint16_t port) {
  bedrock::network::Address addr;
  addr.SetAddr(bedrock::network::IPVersion::kIPV6, "::1", port);

  bedrock::network::Socket sock(bedrock::network::SocketType::kUDP, addr);
  if (sock.Init() != bedrock::network::SocketErrorStatus::kSuccess) {
    cout_mutex.lock();
    std::cout << "[Client]: Error: " << sock.GetErrorMessage() << std::endl;
    cout_mutex.unlock();
    return EXIT_FAILURE;
  }
  cout_mutex.lock();
  std::cout << "[Client]: Client is now talking to: "
            << static_cast<std::string>(addr) << std::endl;
  cout_mutex.unlock();

  std::vector<std::byte> bytes(8192, static_cast<std::byte>(0xAA));

  sock.Write(bytes);

  cout_mutex.lock();
  std::cout << "[Client]: Sent " << bytes.size() << "bytes." << std::endl;
  cout_mutex.unlock();

  auto read = sock.Read(BUFSIZ);
  if (read.status != bedrock::network::SocketErrorStatus::kSuccess) {
    cout_mutex.lock();
    std::cout << "[Client]: Error: " << sock.GetErrorMessage() << std::endl;
    cout_mutex.unlock();
    return EXIT_FAILURE;
  }

  cout_mutex.lock();
  std::cout << "[Client]: Received: " << read.data.second << "bytes from "
            << static_cast<std::string>(addr) << std::endl;
  cout_mutex.unlock();

  if (read.data.first == bytes) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
