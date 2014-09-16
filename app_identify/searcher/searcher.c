#include <regexParse.h>
#include <stdio.h>

/*
 nextTable:

 0 1 2 3 4 5 6 7 8 * * * * * * * * * * * *
|<--------------alphabetSize-------------->|
            
			|<--startOffset

 0 = end state, 1 = id, 2 = user id, 3 = push_dfa_index
 4 = next dfa, 5 = dead state, 6 = dfa_start

struct dfa_graph_t {
	ushort stateNum;
	ushort alphabetSize;
	ushort startOffset;
	uchar  alphabetTable[256];
	ushort nextTable[1];
};
*/
/* TODO: greed or not ? */
int dfaSearch(struct dfa_graph_t *graph, uchar *Text, int len)
{
	uchar *cur = Text;
	uchar *end = Text + len;
	ushort offset = graph->startOffset;
	ushort alphabetSize = graph->alphabetSize;
	ushort *beginstate = graph->nextTable;
	ushort *curState = beginstate;
	uchar  *alphabetTable = graph->alphabetTable;

	while(cur < end) {
		curState = beginstate + alphabetSize * curState[offset+alphabetTable[*cur]];
		if (curState[0]) {
			printf("Match %d, offset %d, state %d\n",
				curState[1], cur-Text, (curState-beginstate)/alphabetSize);
			return curState[1];
		}
		cur++;
	}

	return 0;
}