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

struct element_s *new_sig_element(int id, char *sig)
{
	struct element_s *entry;

	if (strlen(sig) >= MAX_SIG_LENGTH)
		return NULL;

	entry = (struct element_s *)malloc(sizeof(struct element_s));

	if (entry) {
		entry->id = id;
		strcpy(entry->match, sig);
	}

	return entry;
}

