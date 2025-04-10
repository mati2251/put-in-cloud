#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: %s <interface> <up|down>\n", argv[0]);
    return 0;
  }
  char *interface = argv[1];
  char *state = argv[2];
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }
  struct ifreq ifr;
  ifr.ifr_name[0] = '\0';
  if (strlen(interface) > IFNAMSIZ - 1) {
    printf("Interface name too long\n");
    return 1;
  }
  strncpy(ifr.ifr_name, interface, IFNAMSIZ);
  if (strcmp(state, "up") == 0) {
    ifr.ifr_flags |= IFF_UP;
  } else if (strcmp(state, "down") == 0) {
    ifr.ifr_flags &= ~IFF_UP;
  } else {
    printf("Invalid state: %s\n", state);
    return 1;
  }
}
