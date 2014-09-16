#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <queue.h>
#include <types.h>
#include <regexParse.h>
#include <stdlib.h>

#define MAX_APP_LENGTH 16
#define MAX_SIG_LENGTH 128

#define PROTOCOL_UDP 1
#define PROTOCOL_TCP 2
#define PROTOCOL_HTTP 3

#define SIG_MULTI 2
#define SIG_SINGLE 1
#define SIG_REGEX 3

struct sig_s {
	uint id;
	uint proto;
	uint type;
	uint enable;
	uint priority;
	uint dir;
	char match[MAX_SIG_LENGTH];
	STAILQ_ENTRY(sig_s) next;
};
STAILQ_HEAD(sig_list, sig_s);

struct application_s {
	uint id;
	uint category;
	char name[MAX_APP_LENGTH];
    struct sig_list* sigs;
	STAILQ_ENTRY(application_s) next;
};
STAILQ_HEAD(application_list, application_s);

struct app_entry_t *new_app_sig(int id, char *sig);

inline struct sig_list* create_sig_list()
{
    struct sig_list *head =
        (struct sig_list *)malloc(sizeof(struct sig_list));

    if (head != NULL) {
        STAILQ_INIT(head);
    }

    return head;
}

inline struct sig_s* create_sig()
{
    struct sig_s *sig =
        (struct sig_s *)malloc(sizeof(struct sig_s));

    return sig;
}

inline struct application_s* create_application()
{
    struct application_s *app =
        (struct application_s *)malloc(sizeof(struct application_s));

    return app;
}

inline struct application_list* create_application_list()
{
    struct application_list *head =
        (struct application_list *)malloc(sizeof(struct application_list));

    if (head != NULL) {
        STAILQ_INIT(head);
    }

    return head;
}
#endif
