#include "application.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 *  sig http_host=youku&http_ua=nt&http_re=baidu
 */

/* TODO */
void init_http_sig(struct app_entry_list *app_list)
{
    struct app_entry_t *sig_entry;

    sig_entry = new_app_sig(0x00000404, "GET ");
    STAILQ_INSERT_TAIL(app_list, sig_entry, next);

    sig_entry = new_app_sig(0x00000504, "POST ");
    STAILQ_INSERT_TAIL(app_list, sig_entry, next);

    sig_entry = new_app_sig(0x00000a02, "User-Agent");
    STAILQ_INSERT_TAIL(app_list, sig_entry, next);

    sig_entry = new_app_sig(0x00000703, "Referer");
    STAILQ_INSERT_TAIL(app_list, sig_entry, next);

    sig_entry = new_app_sig(0x00000401, "Host");
    STAILQ_INSERT_TAIL(app_list, sig_entry, next);

    return;
}

/* TODO: Ugly?? 
 * How to define http method: GET POST or Head ?
 * */
int get_element_id_from_sig(char *sig)
{
    if (strncmp(sig, "http_host", strlen("http_host")) == 0) {
        return 1; 
    } else if (strncmp(sig, "http_ua", strlen("http_ua")) == 0) {
        return 2;
    } else if (strncmp(sig, "http_re", strlen("http_re")) == 0) {
        return 3;
    } else if (strncmp(sig, "http_url", strlen("http_url")) == 0) {
        return 4;
    } else
        return 0;
}

char *get_key_from_sig(char *sig)
{
    char *cur = sig;
    while(*cur++) 
        if (*cur == '=') 
            return cur + 1;

    return NULL;
}

void http_dump_sig_entrys(struct app_entry_list *app_list)
{
	struct app_entry_t *sig_entry;

    STAILQ_FOREACH(sig_entry, app_list, next) {
        printf("sig id %8x value %s\n",
                sig_entry->appId, sig_entry->regex);
    }

    return;
}

/* TODO: escape character */
int parse_http_sig(struct app_entry_list *app_list, char *sig, int sig_index, int type)
{
    char delims[] = "&";
    char *result = sig;

    result = (char*)strtok(sig, delims);
    while(result != NULL) {
        int sig_id = get_element_id_from_sig(result);
        char *key = get_key_from_sig(result);
	    struct app_entry_t *sig_entry;

        /* TODO: we need a sig.y to parse match_sig 
         *       Then we could statistics sig num,
         *       and pre-allocate enough memory for sig array
         * */
        sig_id = ((sig_id&0xFF)<<24)|sig_index;
        sig_entry = new_app_sig(sig_id, key);
		STAILQ_INSERT_TAIL(app_list, sig_entry, next);
          
        result = (char*)strtok(NULL, delims);
        if (type == SIG_SINGLE && result)
            assert(0);
    }

    return 0;
}
