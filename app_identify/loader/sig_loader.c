#include "application.h"
#include <regexParse.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
struct dfa_graph_t *Regex_Parse(struct app_entry_list *apps);
void Regex_Test(struct app_entry_list *apps, char *Text);
#ifdef __cplusplus
}
#endif
extern void init_http_sig(struct app_entry_list *app_list);
extern int parse_http_sig(struct app_entry_list *app_list, char *sig, int sig_index, int type);
extern void http_dump_sig_entrys(struct app_entry_list *app_list);
int sig_parse_adapter_ac(struct app_entry_list *app_list);

extern FILE* yyin;
extern int yyparse();
struct application_list* app_root = NULL;

struct dfa_graph_t *appgraph;
struct app_entry_list app_list;

void loadApp(const char *file)
{
  yyin = fopen(file, "r");

  if (!yyin)
    exit (2);

  yyparse();

  if (fclose (yyin) != 0)
    exit (3);
}

void dump_applications(struct application_list* root)
{
    struct application_s *app = NULL;
    struct sig_list *sigs = NULL;
    struct sig_s *sig = NULL;

    STAILQ_FOREACH(app, root, next) {
        printf("app\n\tappid    : %d\n", app->id);
        printf("\tcategory : 0x%x\n", app->category);
        printf("\tappname  : %s\n", app->name);
        sigs = app->sigs;
        STAILQ_FOREACH(sig, sigs, next) {
            printf("\tsig:\n\t\tid       : 0x%x\n", sig->id);
            printf("\t\tproto    : %d\n", sig->proto);
            printf("\t\ttype     : %d\n", sig->type);
            printf("\t\tenable   : %d\n", sig->enable);
            printf("\t\tpriority : %d\n", sig->priority);
            printf("\t\tdir      : %d\n", sig->dir);
            printf("\t\tmatch    : %s\n", sig->match);
        }
    }
}

void graph_dump(struct dfa_graph_t *graph)
{
	ushort stateNum = graph->stateNum;
	ushort alphabetSize = graph->alphabetSize;
	uchar  *alphabetTable = graph->alphabetTable;
	ushort *nextTable = graph->nextTable;
	uchar i;
	ushort j;

	printf("graph info:\n\t stateNum: %d\n\talphabetSize: %d\n", stateNum, alphabetSize);

	printf("alphabetTable:\n");

	for(i = 'a'; i <= 'z'; i++){
		printf("%3c ", i);
	}
	printf("\n");

	for (i = 'a'; i <= 'z'; i++) {
		printf("%3d ", alphabetTable[i]);
	}
	printf("\n");


	printf("stateTable:\n");
	for (j = 0; j < stateNum; j++) {
		printf("state %d:\n", j);
		for (i = 0; i < alphabetSize; i++)
			printf("%4d ", nextTable[i]);
		
		printf("\n");
		nextTable += alphabetSize;
	}

}

struct dfa_graph_t *parseApplication(struct application_list* root)
{
	struct application_s *app = NULL;
    struct sig_list *sigs = NULL;
    struct sig_s *sig = NULL;

	STAILQ_INIT(&app_list);
    init_http_sig(&app_list);
    STAILQ_FOREACH(app, root, next) {
        sigs = app->sigs;
        STAILQ_FOREACH(sig, sigs, next) {
			if (sig->proto == PROTOCOL_HTTP) {
                parse_http_sig(&app_list, sig->match,
                        sig->id, sig->type);
            }
		}
	}

    //http_dump_sig_entrys(&app_list);
    sig_parse_adapter_ac(&app_list);
	return Regex_Parse(&app_list);
}

int load_sig(const char *file)
{
    loadApp(file);
    //dump_applications(app_root);
	appgraph = parseApplication(app_root);
    //graph_dump(appgraph);
	return 0;
}

