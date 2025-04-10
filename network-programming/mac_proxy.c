#include <arpa/inet.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define ETH_P_CUSTOM 0x8888

int main(int argc, char **argv) {
  int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_CUSTOM));
  if (sfd < 0) {
    perror("socket: ");
    return 1;
  }

  struct ifreq ifr;
  strncpy(ifr.ifr_name, argv[1], IFNAMSIZ);
  int err = ioctl(sfd, SIOCGIFINDEX, &ifr);
  if (err == -1){
    perror("ioctl: ");
    return 1;
  }

  struct sockaddr_ll sall;
  memset(&sall, 0, sizeof(struct sockaddr_ll));
  sall.sll_family = AF_PACKET;
  sall.sll_protocol = htons(ETH_P_CUSTOM);
  sall.sll_ifindex = ifr.ifr_ifindex;
  sall.sll_hatype = ARPHRD_ETHER;
  sall.sll_pkttype = PACKET_HOST | PACKET_OUTGOING;
  sall.sll_halen = ETH_ALEN;

  err = bind(sfd, (struct sockaddr *)&sall, sizeof(struct sockaddr_ll));
  if (err < 0) {
    perror("bind: ");
  }

  while (1) {
    char *frame = malloc(ETH_FRAME_LEN);
    memset(frame, 0, ETH_FRAME_LEN);
    struct ethhdr *fhead = (struct ethhdr *)frame;
    char *fdata = frame + ETH_HLEN;
    size_t len = recvfrom(sfd, frame, ETH_FRAME_LEN, 0, NULL, NULL);
    if (len < 0) {
      perror("recvfrom: ");
      free(frame);
      continue;
    }
    printf("%s\n", fdata);
    
    char *send_frame = malloc(ETH_FRAME_LEN);
    memset(send_frame, 0, ETH_FRAME_LEN);
    struct ethhdr *send_fhead = (struct ethhdr *)send_frame;
    char *send_fdata = send_frame + ETH_HLEN;

    char send_addr[ETH_ALEN];
    sscanf(argv[2], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &send_addr[0], &send_addr[1], &send_addr[2],
         &send_addr[3], &send_addr[4], &send_addr[5]);
    memcpy(send_fhead->h_dest, send_addr, ETH_ALEN);
    memcpy(send_fhead->h_source, fhead->h_dest, ETH_ALEN);
    send_fhead->h_proto = htons(ETH_P_CUSTOM);
    memcpy(send_fdata, fdata, len - ETH_HLEN);
    err = sendto(sfd, send_frame, ETH_HLEN + len - ETH_HLEN, 0,
                 (struct sockaddr *)&sall, sizeof(struct sockaddr_ll));
    if (err < 0) {
      perror("sendto: ");
    }
    free(send_frame);
    free(frame);
  }
  close(sfd);
  return EXIT_SUCCESS;
}
