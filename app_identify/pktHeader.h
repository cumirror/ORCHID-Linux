#ifndef __PKTHEADER_H__
#define __PKTHEADER_H__

#include <pcap.h>

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
#define IP_FLAG_CE        0x8000  /* Flag: "Congestion"       */
#define IP_FLAG_DF        0x4000  /* Flag: "Don't Fragment"   */
#define IP_FLAG_MF        0x2000  /* Flag: "More Fragments"   */
#define IP_FLAG_OFFSET    0x1FFF  /* "Fragment Offset" part   */
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
#define IP_PROTO_UDP	17
#define IP_PROTO_TCP	6
#define IP_PROTO_ICMP	1
	u_short crc;			// Header checksum
	u_int	saddr;		    // Source address
	u_int	daddr;		    // Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

#define iptlen(iphdr)		(ntohs((iphdr)->tlen))
#define iphdrsize(iphdr)	((iphdr->ver_ihl & 0xf) * 4)
#define ipdatastart(iphdr)	(((u_char*)iphdr) + iphdrsize(iphdr))
#define ipdatalength(iphdr) (ntohs((iphdr)->tlen) - iphdrsize(iphdr))
#define ipfrag(iphdr)		((iphdr)->flags_fo & htons((HS_IP_MF|HS_IP_OFFSET)))

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

#define udpdatastart(udphdr)    (((u_char *)udphdr) + sizeof(udp_header))
#define udplen(udphdr)      (ntohs((udphdr)->len))
#define udpdatalen(udphdr)  (udplen(udphdr) - sizeof(udp_header))


typedef struct tcp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_long seq;
	u_long ack;
	u_char hlen;
	u_char flags;
#define TCP_FLAG_FIN	0x01
#define TCP_FLAG_SYN	0x02
#define TCP_FLAG_RST	0x04
#define TCP_FLAG_PSH	0x08
#define TCP_FLAG_ACK	0x10
#define TCP_FLAG_URG	0x20
#define TCP_FLAG_ECN	0x40
#define TCP_FLAG_CWR	0x80
	u_short window;
	u_short chksum;
	u_short urg;
} tcp_header;

#define tcpdatastart(tcphdr)    (((u_char *)tcphdr) + ((tcphdr->hlen >> 4) << 2))
#define tcphdrsize(tcphdr)  ((tcphdr->hlen >> 4) << 2)
#define tcpdatalength(iphdr, tcphdr) (iptlen(iphdr) - iphdrsize(iphdr) - tcphdrsize(tcphdr))

#endif