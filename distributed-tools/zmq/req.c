#include <stdio.h>
#include <unistd.h>
#include <zmq.h>

int main(void) {
  printf("Connecting to sub-pub server…\n");
  void *context = zmq_ctx_new();
  void *requester = zmq_socket(context, ZMQ_REQ);
  zmq_connect(requester, "tcp://localhost:5555");
  zmq_connect(requester, "tcp://localhost:5556");
  int index = 11111;

  int request_nbr;
  for (request_nbr = 0; request_nbr != 6; request_nbr++) {
    int size = zmq_send(requester, &index, sizeof(int), 0);
    printf("Sent: %d %d\n", index, size);
    char buffer[255];
    int size2 = zmq_recv(requester, buffer, 255, 0);
    printf("Received: %s %d\n", buffer, size2);
  }
  zmq_close(requester);
  zmq_ctx_destroy(context);
  return 0;
}
