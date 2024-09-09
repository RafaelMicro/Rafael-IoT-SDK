/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE:
*/
#define DLT_IEEE802_15_4 195
#define DLT_IEEE802_15_4_NONASK_PHY 215
#define ETH_ALEN 6
#include <sys/time.h>

typedef struct pcap pcap_t;
typedef struct pcap_dumper pcap_dumper_t;
typedef unsigned char u_char;
typedef unsigned int u_int32_t;
typedef unsigned int u_int8_t;
typedef unsigned long bpf_u_int32;
typedef unsigned short int u_int16_t;

struct ether_header {
    u_int8_t ether_dhost[ETH_ALEN];
    u_int8_t ether_shost[ETH_ALEN];
    u_int16_t ether_type;
} __attribute__ ((__packed__));

struct iphdr {
    unsigned int ihl:4;
    unsigned int version:4;
    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
};

struct pcap_pkthdr {
    struct timeval ts;
    bpf_u_int32 caplen;
    bpf_u_int32 len;
};

struct udphdr {
    u_int16_t source;
    u_int16_t dest;
    u_int16_t len;
    u_int16_t check;
};

typedef unsigned long long zb_uint64_t;
