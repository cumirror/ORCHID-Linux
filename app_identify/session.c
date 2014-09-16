#include "session.h"
#include "pktHeader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static flow_table_t flow_table = { 0, NULL };
static struct session_list session_table;
static u_long g_next_sess_id = 0;

/*
 * bob's hash function: full avalanche
 * http://burtleburtle.net/bob/hash/integer.html
 */
static u_int hash(u_int a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16);
    return a;
}

static int hash5tuple(u_long sip, u_long dip, u_short sport, u_short dport, u_char proto)
{
	u_long result;

	result = hash(sip) ^ hash(dip) ^ hash((sport << 16) | dport) ^ hash(proto);
	return result % HASH_TABLE_SIZE;
}

session_t *session_table_find(u_int sip, u_int dip, u_short sport, u_short dport, u_char proto)
{
	u_int hres = hash5tuple(sip, dip, sport, dport, proto);
	u_int hres2;
	flow_t *flow;
	session_t *sess;

	TAILQ_FOREACH(flow, &flow_table.htable[hres], hash_link) {
		if (flow->sip == sip && flow->dip == dip && flow->sport == sport && flow->dport == dport && flow->proto == proto) {
			return flow->psess;
		}
	}

	/* find on the reverse direction */
	hres2 = hash5tuple(dip, sip, dport, sport, proto);
	TAILQ_FOREACH(flow, &flow_table.htable[hres2], hash_link) {
		if (flow->sip == sip && flow->dip == dip && flow->sport == sport && flow->dport == dport && flow->proto == proto) {
			return flow->psess;
		}
	}

	/* not found, create new */
	sess = (session_t *)calloc(1, sizeof(session_t));
	if (!sess) {
		printf("Allocated session failed\n");
		return NULL;
	}

	sess->flow0.sip = sip;
	sess->flow0.dip = dip;
	sess->flow0.sport = sport;
	sess->flow0.dport = dport;
	sess->flow0.proto = proto;
	sess->flow0.flowflag = FLOW_INIT_SIDE;
	sess->flow0.psess = sess;
	sess->flow1.sip = dip;
	sess->flow1.dip = sip;
	sess->flow1.sport = dport;
	sess->flow1.dport = sport;
	sess->flow1.proto = proto;
	sess->flow1.psess = sess;

	sess->session_id = ++g_next_sess_id;

	TAILQ_INSERT_HEAD(&flow_table.htable[hres], &sess->flow0, hash_link);
	TAILQ_INSERT_HEAD(&flow_table.htable[hres2], &sess->flow1, hash_link);
	TAILQ_INSERT_TAIL(&session_table, sess, hash_link);

	return sess;
}

int create_session_table()
{
	int i;

	flow_table.htable = (struct flow_list *)malloc(sizeof(struct flow_list) * HASH_TABLE_SIZE);
	if (!flow_table.htable) {
		printf("Allocate flow_table failed\n");
		return -1;
	}
	for (i = 0; i < HASH_TABLE_SIZE; i++) {
		TAILQ_INIT(&flow_table.htable[i]);
	}
	flow_table.table_size = HASH_TABLE_SIZE;

	TAILQ_INIT(&session_table);
	return 0;
}

int destroy_dp_session_table()
{
	/* TODO */
	return -1;
}

#define IP2STRING(ip) \
	((u_char*)&(ip))[0],   \
	((u_char*)&(ip))[1],   \
	((u_char*)&(ip))[2],   \
	((u_char*)&(ip))[3]

void dump_session_table()
{
	session_t *sess;
	flow_t* flow;


	TAILQ_FOREACH(sess, &session_table, hash_link) {
		flow = &sess->flow0;
		printf("%s %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d : appid %d\n", (flow->proto == IP_PROTO_TCP) ? "TCP":"UDP",
			IP2STRING(flow->sip), flow->sport, IP2STRING(flow->dip), flow->dport, sess->app_id);
	}

	return ;
}