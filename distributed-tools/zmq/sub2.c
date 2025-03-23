#include <stdio.h>
#include <unistd.h>
#include <zmq.h>

int main(void) {
  printf("Connecting to sub-pub server…\n");
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_SUB);
  zmq_setsockopt(requester, ZMQ_SUBSCRIBE, "", 0);
  zmq_connect(requester, "tcp://localhost:6005");

  int request_nbr;
  for (request_nbr = 0; request_nbr != 3; request_nbr++) {
    char buffer[255];
    int size = zmq_recv(requester, buffer, 255, 0);
    buffer[size] = '\0';
    printf("Received: %s %d\n", buffer, size);
  }
  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
