#include <libnet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <pcap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

u_int32_t target_ip;

void trap(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
  u_int32_t *addr = (u_int32_t *)(bytes + 28);
  if (*addr == target_ip) {
    char *target = inet_ntoa(*(struct in_addr *)&target_ip);
    printf("Success: %s\n", target);
    exit(0);
  }
}

int main(int argc, char **argv) {
  bpf_u_int32 netp, maskp;
  struct bpf_program fp;
  char *pcap_errbuf = malloc(PCAP_ERRBUF_SIZE);
  pcap_t *handle = pcap_create(argv[1], pcap_errbuf);
  if (handle == NULL) {
    printf("pcap_create(): %s\n", pcap_errbuf);
    return 1;
  }
  pcap_set_promisc(handle, 1);
  pcap_set_snaplen(handle, 65535);
  pcap_set_timeout(handle, 1000);
  pcap_activate(handle);
  int err = pcap_lookupnet(argv[1], &netp, &maskp, pcap_errbuf);
  if (err == -1) {
    printf("pcap_lookupnet(): %s\n", pcap_errbuf);
    return 1;
  }
  pcap_compile(handle, &fp, "arp", 0, maskp);
  if (pcap_setfilter(handle, &fp) < 0) {
    pcap_perror(handle, "pcap_setfilter()");
    return 1;
  }

  libnet_t *ln;
  u_int32_t src_ip_addr;
  struct libnet_ether_addr *src_hw_addr;
  char errbuf[LIBNET_ERRBUF_SIZE];

  ln = libnet_init(LIBNET_LINK, argv[1], errbuf);
  src_ip_addr = libnet_get_ipaddr4(ln);
  src_hw_addr = libnet_get_hwaddr(ln);
  target_ip = libnet_name2addr4(ln, argv[2], LIBNET_RESOLVE);

  u_int8_t zero_hw_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  libnet_autobuild_arp(ARPOP_REQUEST, src_hw_addr->ether_addr_octet,
                       (u_int8_t *)&src_ip_addr, zero_hw_addr,
                       (u_int8_t *)&target_ip, ln);
  u_int8_t bcast_hw_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  libnet_autobuild_ethernet(bcast_hw_addr, ETHERTYPE_ARP, ln);

  printf("Sending...\n");
  libnet_write(ln);
  pcap_loop(handle, 5, trap, NULL);
  libnet_destroy(ln);
  return 1;
}
