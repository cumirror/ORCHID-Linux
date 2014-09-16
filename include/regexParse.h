#ifndef __REGEX_PARSE_H
#define __REGEX_PARSE_H

#include <queue.h>

typedef unsigned short ushort;
typedef unsigned char uchar;

struct app_entry_t {
	int	 appId;
	char regex[128];
	STAILQ_ENTRY(app_entry_t) next;
};
STAILQ_HEAD(app_entry_list, app_entry_t);

/*
 nextTable:

 0 1 2 3 4 5 6 7 8 * * * * * * * * * * * *
|<--------------alphabetSize-------------->|
            
			|<--startOffset

 0 = end state, 1 = id, 2 = user id, 3 = push_dfa_index
 4 = next dfa, 5 = dead state, 6 = dfa_start
*/
struct dfa_graph_t {
	ushort stateNum;
	ushort alphabetSize;
	ushort startOffset;
	uchar  alphabetTable[256];
	ushort nextTable[1];
};

#endif
