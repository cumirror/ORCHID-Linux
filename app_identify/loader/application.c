#include <string.h>
#include "application.h"

struct app_entry_t *new_app_sig(int id, char *sig)
{
	struct app_entry_t *entry;

	if (strlen(sig) >= MAX_SIG_LENGTH)
		return NULL;

	entry = (struct app_entry_t *)malloc(sizeof(struct app_entry_t));

	if (entry) {
		entry->appId = id;
		strcpy(entry->regex, sig);
	}

	return entry;
}

