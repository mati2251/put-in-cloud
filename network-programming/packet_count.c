#include <linux/if_ether.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <pcap.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

char *errbuf;
pcap_t *handle;

void cleanup() {
  pcap_close(handle);
  free(errbuf);
}

long int ip = 0;
long int ip_udp = 0;
long int ip_tcp = 0;
long int arp = 0;
long int rest = 0;

void stop(int signo) {
  printf("\n");
  printf("IP: %ld\n", ip);
  printf("IP UDP: %ld\n", ip_udp);
  printf("IP TCP: %ld\n", ip_tcp);
  printf("ARP: %ld\n", arp);
  printf("Rest: %ld\n", rest);
  exit(EXIT_SUCCESS);
}

void trap(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
  struct ethhdr *eth = (struct ethhdr *)bytes;
  if (ntohs(eth->h_proto) == ETH_P_ARP) {
    arp++;
  } else if (ntohs(eth->h_proto) == ETH_P_IP) {
    ip++;
    struct iphdr *ip_hdr = (struct iphdr *)(bytes + sizeof(struct ethhdr));
    if (ip_hdr->protocol == IPPROTO_TCP) {
      ip_tcp++;
    } else if (ip_hdr->protocol == IPPROTO_UDP) {
      ip_udp++;
    }
  } else {
    rest++;
  }
}

int main(int argc, char **argv) {
  bpf_u_int32 netp, maskp;
  struct bpf_program fp;
  atexit(cleanup);
  signal(SIGINT, stop);
  errbuf = malloc(PCAP_ERRBUF_SIZE);
  handle = pcap_create(argv[1], errbuf);
  if (handle == NULL) {
    printf("%s", errbuf);
  }
  pcap_set_promisc(handle, 1);
  pcap_set_snaplen(handle, 65535);
  pcap_set_timeout(handle, 1000);
  pcap_activate(handle);
  int err = pcap_lookupnet(argv[1], &netp, &maskp, errbuf);
  if (err == -1) {
    printf("%s", errbuf);
  }
  pcap_loop(handle, -1, trap, NULL);
}
