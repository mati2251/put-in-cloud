/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "counter.h"

void counter_1(char *host, int action, int value) {
  CLIENT *clnt;
  int *result_1;
  int down_1_arg = value;
  int *result_2;
  int up_1_arg = value;

#ifndef DEBUG
  clnt = clnt_create(host, COUNTER, V1, "udp");
  if (clnt == NULL) {
    clnt_pcreateerror(host);
    exit(1);
  }
#endif /* DEBUG */

  if (1 == action) {
    result_1 = up_1(&up_1_arg, clnt);
    if (result_1 == (int *)NULL) {
      clnt_perror(clnt, "call failed");
      return;
    }
    printf("%d\n", *result_1);
  } else if (action == 2) {
    result_2 = down_1(&down_1_arg, clnt);
    if (result_2 == (int *)NULL) {
      clnt_perror(clnt, "call failed");
      return;
    } else {
      printf("%d\n", *result_2);
    }
  }
#ifndef DEBUG
  clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[]) {
  char *host;

  if (argc < 4) {
    printf("usage: %s server_host action value\n", argv[0]);
    exit(1);
  }
  host = argv[1];
  counter_1(host, atoi(argv[2]), atoi(argv[3]));
  exit(0);
}
