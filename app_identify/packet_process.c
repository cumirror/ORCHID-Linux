#include "pktHeader.h"
#include "session.h"

extern int dfaSearch(struct dfa_graph_t *graph, uchar *Text, int len);
extern int httpParse(struct dfa_graph_t *graph, uchar *Text, int len);
extern struct dfa_graph_t *appgraph;

void packet_handler(u_char *param, const struct pcap_pkthdr *pkthdr, const u_char *pktdata)
{
	ip_header *ih;
	int ip_len;
	udp_header *uh;
	tcp_header *th;
	u_short sport,dport;
	session_t *sess;
	u_char *payload;
	int payload_len = pkthdr->caplen;

	/*
	* unused parameters
	*/
	(void)(param);

	if (*(pktdata + 12) != 0x08 || *(pktdata + 13) != 0x00) {
		/* TODO: Support vlan ? */
		return;
	}

	/* retireve the position of the ip header */
	ih = (ip_header *) (pktdata + 14);
	payload_len -= 14;

	/* retireve the position of the udp header */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	payload_len -= ip_len;
	switch (ih->proto) {
	case IP_PROTO_TCP:
		th = (tcp_header *) ((u_char*)ih + ip_len);
		/* convert from network byte order to host byte order */
		sport = th->sport;
		dport = th->dport;
		payload = tcpdatastart(th);
		payload_len -= tcphdrsize(th);
		break;
	case IP_PROTO_UDP:
		uh = (udp_header *) ((u_char*)ih + ip_len);
		/* convert from network byte order to host byte order */
		sport = uh->sport;
		dport = uh->dport;
		payload = (u_char*)uh + sizeof(udp_header);
		payload_len -= sizeof(udp_header);
		break;
	default:
		return;
	}

	sess = session_table_find(ih->saddr, ih->daddr, sport, dport, ih->proto);

	if (sess->app_id == 0) {
		if (payload_len > 0)
			sess->app_id = httpParse(appgraph, payload, payload_len);
	}

	return;
}
