#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IRI_T_ADDRESS 0
#define IRI_T_ROUTE 1
struct ifrtinfo {
  int iri_type;
  char iri_iname[16];
  struct sockaddr_in iri_iaddr;
  struct sockaddr_in iri_rtdst;
  struct sockaddr_in iri_rtmsk;
  struct sockaddr_in iri_rtgip;
};

#define ETH_P_CUSTOM 0x8888

int main(int argc, char **argv) {
  int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_CUSTOM));
  if (sfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }
  struct ifreq ifr;
  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  int err = ioctl(sfd, SIOCGIFINDEX, &ifr);
  if (err < 0) {
    perror("ioctl");
    close(sfd);
    return EXIT_FAILURE;
  }
  struct sockaddr_ll sall;
  memset(&sall, 0, sizeof(struct sockaddr_ll));
  sall.sll_family = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_CUSTOM);
  sall.sll_ifindex = ifr.ifr_ifindex;
  sall.sll_hatype = ARPHRD_ETHER;
  sall.sll_pkttype = PACKET_HOST;
  sall.sll_halen = ETH_ALEN;
  err = bind(sfd, (struct sockaddr *)&sall, sizeof(struct sockaddr_ll));
  if (err < 0) {
    perror("bind");
    close(sfd);
    return EXIT_FAILURE;
  }
  int ssfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (ssfd < 0) {
    perror("socket");
    close(sfd);
    return EXIT_FAILURE;
  }
  while (1) {
    char *frame = malloc(ETH_FRAME_LEN);
    memset(frame, 0, ETH_FRAME_LEN);
    struct ethhdr *fhead = (struct ethhdr *)frame;
    char *fdata = frame + ETH_HLEN;
    ssize_t len = recvfrom(sfd, frame, ETH_FRAME_LEN, 0, NULL, NULL);
    struct ifrtinfo *info;
    info = (struct ifrtinfo *)fdata;

    if (info->iri_type == IRI_T_ADDRESS) {
      printf("Received IRI_T_ADDRESS\n");
      struct ifreq ifr;
      struct sockaddr_in *sin;
      strncpy(ifr.ifr_name, info->iri_iname, strlen(info->iri_iname) + 1);
      memcpy(&ifr.ifr_addr, &info->iri_iaddr, sizeof(struct sockaddr_in));
      err = ioctl(ssfd, SIOCSIFADDR, &ifr);
      if (err < 0) {
        perror("ioctl");
        free(frame);
        continue;
      }
      err = ioctl(ssfd, SIOCGIFFLAGS, &ifr);
      if (err < 0) {
        perror("ioctl");
        free(frame);
        continue;
      }
      ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
      err = ioctl(ssfd, SIOCSIFFLAGS, &ifr);
      if (err < 0) {
        perror("ioctl");
        free(frame);
        continue;
      }
      char ip_str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &info->iri_iaddr.sin_addr, ip_str, sizeof(ip_str));
      printf("IP interface %s set to %s\n", info->iri_iname, ip_str);
    } else if (info->iri_type == IRI_T_ROUTE) {

      printf("Received IRI_T_ROUTE\n");
      struct rtentry route;
      memset(&route, 0, sizeof(route));
      struct sockaddr_in *addr;
      memcpy(&route.rt_gateway, &info->iri_rtgip, sizeof(struct sockaddr_in));
      memcpy(&route.rt_genmask, &info->iri_rtmsk, sizeof(struct sockaddr_in));
      memcpy(&route.rt_dst, &info->iri_rtdst, sizeof(struct sockaddr_in));

      route.rt_flags = RTF_UP | RTF_GATEWAY;
      route.rt_metric = 0;
      int err = ioctl(sfd, SIOCADDRT, &route);
      if (err < 0) {
        perror("ioctl");
        close(sfd);
        return EXIT_FAILURE;
      }

    } else {
      printf("Unknown type: %d\n", info->iri_type);
    }
    free(frame);
  }
  close(sfd);
  return EXIT_SUCCESS;
}
