#include "server.h"

::grpc::Status CounterServiceImpl::up(::grpc::ServerContext *context,
                                      const ::Counter *request,
                                      ::Counter *response) {
  this->counter.set_value(this->counter.value() + request->value());
  std::cout << "Counter value: " << this->counter.value() << std::endl;
  response->set_value(this->counter.value());
  return ::grpc::Status::OK;
}

::grpc::Status CounterServiceImpl::down(::grpc::ServerContext *context,
                                        const ::Counter *request,
                                        ::Counter *response) {
  this->counter.set_value(this->counter.value() - request->value());
  std::cout << "Counter value: " << this->counter.value() << std::endl;
  response->set_value(this->counter.value());
  return ::grpc::Status::OK;
}

int main() {
  CounterServiceImpl service;
  grpc::ServerBuilder builder;
  std::string server_address("0.0.0.0:8888");
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}
