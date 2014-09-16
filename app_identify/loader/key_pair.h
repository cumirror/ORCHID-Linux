#ifndef __KEY_PAIR_H__
#define __KEY_PAIR_H__

#include "application.h"
#include <queue.h>
#include <types.h>
#include <stdlib.h>
#include <string.h>

struct key_pair_s {
	char* key;
	char* value;
	STAILQ_ENTRY(key_pair_s) next;
};
STAILQ_HEAD(key_pair_list, key_pair_s);

inline struct key_pair_list* create_pair_list()
{
    struct key_pair_list *head =
        (struct key_pair_list *)malloc(sizeof(struct key_pair_list));

    if (head != NULL) {
        STAILQ_INIT(head);
    }

    return head;
}

inline void free_pair_list(struct key_pair_list *head)
{
    struct key_pair_s *pair = NULL;
    struct key_pair_s *tpair = NULL;

    STAILQ_FOREACH_SAFE(pair, head, next, tpair) {
        free(pair->key);
        free(pair->value);
        free(pair);
    }
    free(head);
}

inline struct key_pair_s* create_key_value_pair(char* key, char* value)
{
    struct key_pair_s *pair = NULL;
   
    pair = (struct key_pair_s *)malloc(sizeof(struct key_pair_s));

    if (pair != NULL) {
       pair->key = key;
       pair->value = value;
    }

    return pair;
}

inline uint get_int_value_from_key(struct key_pair_list* head, const char* key)
{
    struct key_pair_s *pair; 
    uint value = 0;

    STAILQ_FOREACH(pair, head, next) {
        if (strcmp(pair->key, key) == 0) {
            value = strtol(pair->value, NULL, 0);            
            break;
        }
    }

    return value;
}

inline char* get_str_value_from_key(struct key_pair_list* head, const char* key)
{
    struct key_pair_s *pair; 
    char *value = NULL;

    STAILQ_FOREACH(pair, head, next) {
        if (strcmp(pair->key, key) == 0) {
            value = pair->value;            
            break;
        }
    }

    return value;
}

inline static uint proto_to_value(char *proto)
{
    if (strcmp(proto, "UDP") == 0)
        return PROTOCOL_UDP;
    if (strcmp(proto, "TCP") == 0)
        return PROTOCOL_TCP;
    if (strcmp(proto, "HTTP") == 0)
        return PROTOCOL_HTTP;

    return 0;
}

inline uint get_sigProto_value(struct key_pair_list* head)
{
    struct key_pair_s *pair; 
    uint value = 0;

    STAILQ_FOREACH(pair, head, next) {
        if (strcmp(pair->key, "sig_proto") == 0) {
            value = proto_to_value(pair->value);            
            break;
        }
    }

    return value;
}

inline static uint matchType_to_value(char *type)
{
    if (strcmp(type, "AC_SINGLE") == 0)
        return SIG_SINGLE;
    if (strcmp(type, "AC_MULTI") == 0)
        return SIG_MULTI;
    if (strcmp(type, "REGEX") == 0) 
        return SIG_REGEX;

    return 0;
}

inline uint get_matchType_value(struct key_pair_list* head)
{
    struct key_pair_s *pair; 
    uint value = 0;

    STAILQ_FOREACH(pair, head, next) {
        if (strcmp(pair->key, "sig_type") == 0) {
            value = matchType_to_value(pair->value);            
            break;
        }
    }

    return value;
}

#endif
