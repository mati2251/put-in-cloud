#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main(void) {
  printf("Connecting to pull server…\n");
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_PUSH);
  zmq_connect(requester, "tcp://localhost:6001");

  char *name = "John Doe";
  int request_nbr;
  for (request_nbr = 0; request_nbr != 3; request_nbr++) {
    int size = zmq_send(requester, name, strlen(name), 0);
    printf("Sent: %s %d\n", name, size);
  }
  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
