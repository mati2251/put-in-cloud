#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv) {
  int sfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (sfd < 0) {
    perror("socket");
    return 1;
  }
  struct ifconf ifc;
  struct ifreq ifr[50];
  ifc.ifc_len = sizeof(ifr);
  ifc.ifc_req = ifr;
  if (ioctl(sfd, SIOCGIFCONF, &ifc) < 0) {
    perror("ioctl");
    close(sfd);
    return 1;
  }
  int num_interfaces = ifc.ifc_len / sizeof(struct ifreq);
  for (int i = 0; i < num_interfaces; i++) {
    struct ifreq* item = &ifr[i];
    if (ioctl(sfd, SIOCGIFHWADDR, item) < 0) {
      perror("ioctl");
      close(sfd);
      return 1;
    }
    unsigned char* mac = (unsigned char*)item->ifr_hwaddr.sa_data;
    printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", item->ifr_name,
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
  return 0;
}
