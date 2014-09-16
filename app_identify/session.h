#ifndef __SESSION_H__
#define __SESSION_H__
#include <queue.h>
#include <types.h>

/*
static int ht_size[] = {
	37, 47, 79, 97, 163, 197, 331, 397, 673, 797, 1361, 1597,
	2729, 3203, 5471, 6421, 10949, 12853, 21911, 25717, 43853,
	51437, 87719, 102877, 175447, 205759, 350899, 411527, 701819,
	823117, 1403641, 1646237, 2807303, 3292489, 5614657, 6584983,
	11229331, 13169977, 22458671, 26339969, 44917381, 52679969,
	89834777, 105359939
};
*/
#define HASH_TABLE_SIZE		5471

typedef struct flow_s flow_t;
typedef struct session_s session_t;

struct flow_s {
	session_t *psess;
#define FLOW_INIT_SIDE 0x0
#define FLOW_RESP_SIDE 0x1
	u_long flowflag;
	u_int sip;
	u_int dip;
	u_short sport;
	u_short dport;
	u_char proto;
	TAILQ_ENTRY(flow_s) hash_link;
};

#define flow2session(pflow) ((pflow)->psess)

typedef struct session_s {
	flow_t flow0;
	flow_t flow1;
	unsigned int session_id;
	unsigned short app_id;
#define SESSION_APPID_DONE		0x00000001
	unsigned short sflag;
	TAILQ_ENTRY(session_s) hash_link;
} session_t;

TAILQ_HEAD(flow_list, flow_s);
TAILQ_HEAD(session_list, session_s);

typedef struct dp_flow_table_s {
	int table_size;
	struct flow_list *htable;
} flow_table_t;

int create_session_table();
int destroy_session_table(void);
session_t *session_table_find(u_int sip, u_int dip, u_short sport, u_short dport, u_char proto);
void dump_session_table();

#endif