#include "../loader/acsmx2.h"
#include "../loader/application.h"
#include "common.h"
#include <regexParse.h>
#include <types.h>
#include <stdio.h>
#include <assert.h>

int pattern_count = 0;
int pattern[100] = {0};

#define IS_HTTP_ELEMENT(x) (((x)&0xFF000000) == 0x0)
#define HTTP_ELEMENT_ID(x) (((x)&0xFF000000) ? (((x)>>24)&0xFF) : ((x)&0xFF))
#define HTTP_ELEMENT_LENGTH(x) (((x)&0x00FFFF00)>>8)

/*
 *
 *--------
 *       |
 *       |
 *       |
 *       |
 *--------
 *
 *
 *
 * */

#define EXP_ARRYR_SIZE 8
int pattern_check(int *patterns)
{
    int exp_array[EXP_ARRYR_SIZE] = {0};
    int exp_num = 0;
    struct sig_entry_s *sigs, *cur_sig;
    struct element_entry_s *eles, *cur_ele;
    int ele_offset = 0;
    int i,j;

    sigs = app_array.sigs;
    eles = app_array.eles;

    /* enable? priority? multi_pattern? */
    for (i = 0; i < pattern_count; i++) {
        ele_offset = patterns[i]&0xFFFF;
        cur_ele = eles+ele_offset;
        if (cur_ele->flag == (ELEMENT_END | ELEMENT_START)) {
            printf("Match APP %x Category %x : sig index %x\n", 
                    sigs[cur_ele->sigindex].sigid & 0xFFFF,
                    (sigs[cur_ele->sigindex].sigid & 0xFFFF0000) >> 16,
                    cur_ele->sigindex);
            exp_array[exp_num++] = cur_ele->exp_ele_index;
        } else if (cur_ele->flag & ELEMENT_START) {
            exp_array[exp_num++] = cur_ele->exp_ele_index;
        } else {
            for (j = 0; j < exp_num; j++) {
                if (exp_array[j] == ele_offset) {
                    if (cur_ele->flag & ELEMENT_END) {
                        printf("Match APP %x Category %x : sig index %x\n", 
                                sigs[cur_ele->sigindex].sigid & 0xFFFF,
                                (sigs[cur_ele->sigindex].sigid & 0xFFFF0000) >> 16,
                                cur_ele->sigindex);
                    }
                    exp_array[j] = cur_ele->exp_ele_index;
                }
            }
        }

        if (exp_num == EXP_ARRYR_SIZE) break;
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
			    printf("element id %d last id %d, pattern id %x, index %d\n",
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

/*
*    A Match is found
*/
static u_int elementlast = 0;
int MatchFound (void * id, void *tree, int index, void *data, void *neg_list)
{
    u_int match_id = (u_int)id;
    u_int element_id = HTTP_ELEMENT_ID(match_id);

    if (IS_HTTP_ELEMENT(match_id)) {
        if (index == 0 || *((char*)(data - 1)) == '\n') {
            elementlast = element_id;
            printf("Ele begin: id %4d match id 0x%x -- %-16.8s\n", element_id, match_id, (char*)data);
        }
    } else {
        if (element_id == elementlast) {
            pattern[pattern_count++] = match_id;
            printf("Ele match: id %4d match id 0x%x -- %-16.12s\n", element_id, match_id, (char*)data);
        }
    }

    return 0;
}

int httpParse2(ACSM_STRUCT2 *graph, uchar *Text, int len)
{
    int cur_state = 0;
    pattern_count = 0;
    acsmSearch2 (graph, Text, len, MatchFound, (void *)0, &cur_state);
    pattern_check(pattern);
	return 0;
}
