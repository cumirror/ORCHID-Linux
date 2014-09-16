#include <regexParse.h>
#include <types.h>
#include <stdio.h>
#include <assert.h>

int pattern_count = 0;
int pattern[10] = {0};

#define IS_HTTP_ELEMENT(x) (((x)&0xFF000000) == 0x0)
#define HTTP_ELEMENT_ID(x) (((x)&0xFF000000) ? (((x)>>24)&0xFF) : ((x)&0xFF))
#define HTTP_ELEMENT_LENGTH(x) (((x)&0x00FFFF00)>>8)

int pattern_check(int *patterns)
{
    /* enable? priority? multi_pattern? */
    int i;

    for (i = 0; i < pattern_count; i++) {
        printf("pattern %d\n", pattern[i]);
    }

    return 0;
}

int httpParse(struct dfa_graph_t *graph, uchar *Text, int len)
{
	uchar *cur = Text;
	uchar *end = Text + len;
	ushort offset = graph->startOffset;
	ushort alphabetSize = graph->alphabetSize;
	ushort *beginstate = graph->nextTable;
	ushort *curState = beginstate;
	uchar  *alphabetTable = graph->alphabetTable;
    int element_last = 0;

	while(cur < end) {

		curState = beginstate + alphabetSize * curState[offset+alphabetTable[*cur]];
		if (curState[0]) {
            u_int match_id = *((u_int*)(curState+1));
            u_int element_id = HTTP_ELEMENT_ID(match_id);

			//printf("Match %x, offset %d, state %d\n",
			//	match_id, cur-Text, (curState-beginstate)/alphabetSize);

            if (IS_HTTP_ELEMENT(match_id)) {
                /* offset, get element ptr */
                int len = HTTP_ELEMENT_LENGTH(match_id);
                int offset = cur-Text+1;
                int start = offset - len;

			    //printf("len %d, offset %d, start %d\n",
				//    len, offset, start);

                assert(start >= 0);

                if (start == 0 || Text[start-1] == '\n') {
                    element_last = element_id;
                    //http_head[element_id] = Text + start;
                }
            } else {
			    printf("element id %d last id %d, pattern id %d, index %d\n",
				    element_id, element_last, match_id, pattern_count);
                if (element_id == element_last) {
                    pattern[pattern_count++] = curState[1];
                }
            }
        }
		cur++;
	}

	return 0;
}
