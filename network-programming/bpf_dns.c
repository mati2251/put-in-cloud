#include <arpa/inet.h>
#include <linux/filter.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define ETH_P_CUSTOM 0x8888

struct sock_filter filter[] = {
    {0x28, 0, 0, 0x0000000c}, {0x15, 0, 4, 0x000086dd},
    {0x30, 0, 0, 0x00000014}, {0x15, 0, 11, 0x00000011},
    {0x28, 0, 0, 0x00000038}, {0x15, 8, 9, 0x00000035},
    {0x15, 0, 8, 0x00000800}, {0x30, 0, 0, 0x00000017},
    {0x15, 0, 6, 0x00000011}, {0x28, 0, 0, 0x00000014},
    {0x45, 4, 0, 0x00001fff}, {0xb1, 0, 0, 0x0000000e},
    {0x48, 0, 0, 0x00000010}, {0x15, 0, 1, 0x00000035},
    {0x6, 0, 0, 0x00040000},  {0x6, 0, 0, 0x00000000}};

struct sock_fprog bpf = {.len = (sizeof(filter) / sizeof(filter[0])),
                         .filter = filter};

int main(int argc, char **argv) {

  int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }
  int err = setsockopt(sfd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
  if (err < 0) {
    perror("setsockopt");
    return EXIT_FAILURE;
  }
  struct ifreq ifr;
  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  err = ioctl(sfd, SIOCGIFINDEX, &ifr);
  if (err < 0) {
    perror("ioctl");
    return EXIT_FAILURE;
  }
  struct sockaddr_ll sall;
  memset(&sall, 0, sizeof(struct sockaddr_ll));
  sall.sll_family = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_ALL);
  sall.sll_ifindex = ifr.ifr_ifindex;
  sall.sll_hatype = ARPHRD_ETHER;
  sall.sll_pkttype = PACKET_HOST;
  sall.sll_halen = ETH_ALEN;

  while (1) {
    char* frame = malloc(ETH_FRAME_LEN);
    memset(frame, 0, ETH_FRAME_LEN);
    struct ethhdr *fhead = (struct ethhdr *)frame;
    char* fdata = frame + ETH_HLEN;
    size_t len = recvfrom(sfd, frame, ETH_FRAME_LEN, 0, (struct sockaddr *)&sall,
                   &(socklen_t){sizeof(struct sockaddr_ll)});
    printf("Src: %02x:%02x:%02x:%02x:%02x:%02x\n", fhead->h_source[0],
           fhead->h_source[1], fhead->h_source[2], fhead->h_source[3],
           fhead->h_source[4], fhead->h_source[5]);
    printf("Drc: %02x:%02x:%02x:%02x:%02x:%02x\n", fhead->h_dest[0],
           fhead->h_dest[1], fhead->h_dest[2], fhead->h_dest[3],
           fhead->h_dest[4], fhead->h_dest[5]);
    printf("Ether type: %04x\n", ntohs(fhead->h_proto));
    printf("Type packet: %d\n", sall.sll_pkttype);
    printf("\n\n");
    free(frame);
  }
  close(sfd);
  return EXIT_SUCCESS;
}
