#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

int main(void) {
  void *context = zmq_ctx_new();
  void *responder = zmq_socket(context, ZMQ_PUB);
  int rc = zmq_bind(responder, "tcp://*:5555");
  assert(rc == 0);

  while (1) {
    char buffer[255];
    int size = scanf("%s", buffer);
    zmq_send(responder, buffer, strlen(buffer), 0);
    printf("Sent: %s - %lu bytes\n", buffer, strlen(buffer));
  }
  return 0;
}
