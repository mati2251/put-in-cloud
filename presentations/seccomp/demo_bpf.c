#include <arpa/inet.h>
#include <linux/filter.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static struct sock_filter bpf_filter[] = {
{ 0x28, 0, 0, 0x0000000c },
{ 0x15, 0, 9, 0x00000800 },
{ 0x30, 0, 0, 0x00000017 },
{ 0x15, 0, 15, 0x00000006 },
{ 0x28, 0, 0, 0x00000014 },
{ 0x45, 13, 0, 0x00001fff },
{ 0xb1, 0, 0, 0x0000000e },
{ 0x48, 0, 0, 0x0000000e },
{ 0x15, 9, 0, 0x00000050 },
{ 0x48, 0, 0, 0x00000010 },
{ 0x15, 7, 8, 0x00000050 },
{ 0x15, 0, 7, 0x000086dd },
{ 0x30, 0, 0, 0x00000014 },
{ 0x15, 0, 5, 0x00000006 },
{ 0x28, 0, 0, 0x00000036 },
{ 0x15, 2, 0, 0x00000050 },
{ 0x28, 0, 0, 0x00000038 },
{ 0x15, 0, 1, 0x00000050 },
{ 0x6, 0, 0, 0x00040000 },
{ 0x6, 0, 0, 0x00000000 },
};

int main(void) {
  int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  struct sock_fprog prog = {
      .len = sizeof(bpf_filter) / sizeof(bpf_filter[0]),
      .filter = bpf_filter,
  };

  if (setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &prog, sizeof(prog)) < 0) {
    perror("setsockopt SO_ATTACH_FILTER");
    return 1;
  }

  printf("Listening for UDP port 53 (DNS) packets...\n");

  unsigned char buf[2048];
  while (1) {
    ssize_t n = recv(sock, buf, sizeof(buf), 0);
    if (n < 0) {
      perror("recv");
      break;
    }
    printf("DNS packet captured: %zd bytes\n", n);
  }

  close(sock);
  return 0;
}
