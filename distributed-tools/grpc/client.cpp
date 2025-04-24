#include "counter.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <action> <value>" << std::endl;
    return 1;
  }
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateChannel("localhost:8888", grpc::InsecureChannelCredentials());
  std::unique_ptr<CounterService::Stub> stub = CounterService::NewStub(channel);
  Counter request;
  Counter response;
  grpc::ClientContext context;
  request.set_value(std::stoi(argv[2]));
  grpc::Status status;
  if (std::string(argv[1]) == "up") {
    status = stub->up(&context, request, &response);
  } else if (std::string(argv[1]) == "down") {
    status = stub->down(&context, request, &response);
  } else {
    std::cerr << "Invalid action. Use 'up' or 'down'." << std::endl;
    return 1;
  }
  if (status.ok()) {
    std::cout << "Counter value: " << response.value() << std::endl;
  } else {
    std::cerr << "RPC failed" << std::endl;
  }
}
