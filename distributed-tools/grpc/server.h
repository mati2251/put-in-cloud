#include "counter.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

class CounterServiceImpl : public CounterService::Service {
private:
  ::Counter counter;

public:
  ::grpc::Status up(::grpc::ServerContext *context, const ::Counter *request,
                    ::Counter *response);
  ::grpc::Status down(::grpc::ServerContext *context, const ::Counter *request,
                      ::Counter *response);
};
