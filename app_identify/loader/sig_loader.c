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

#define TEST_AC
#define TEST_REGEX

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
    struct element_s *element = NULL;

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
            STAILQ_FOREACH(element, &(sig->elements), next) {
                printf("\t\t\telement id %d : match %s\n",
                        element->id, element->match);
            }
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
	return Regex_Parse(&app_list);
}

struct app_statistics_s app_summary;
int get_Application_summary(struct application_list* root)
{
	struct application_s *app = NULL;
    struct sig_s *sig = NULL;
    struct element_s *element = NULL;
    int i, cate_num;

    STAILQ_FOREACH(app, root, next) {
        app_summary.app_nums++;
        app_summary.cate_num[app->category]++;
        STAILQ_FOREACH(sig, app->sigs, next) {
            app_summary.sig_nums++;
            STAILQ_FOREACH(element, &(sig->elements), next) {
                app_summary.ele_nums++;
            }
		}
	}

    app_summary.cate_index[0] = 0;
    cate_num = app_summary.cate_num[0];
    for (i = 1; i < MAX_CATEGORY_ID; i++) {
        app_summary.cate_index[i] = cate_num;
        cate_num += app_summary.cate_num[i];
    }

    printf("total app nums: %d\n", app_summary.app_nums);
    printf("total sig nums: %d\n", app_summary.sig_nums);
    printf("total ele nums: %d\n", app_summary.ele_nums);
    for (i = 0; i < MAX_CATEGORY_ID; i++) {
        printf("\tcate %d: index %d nums %d\n",
                i, app_summary.cate_index[i], app_summary.cate_num[i]);
    }

	return 0;
}

struct app_info_s app_array;
int install_Application(struct application_list* root)
{
    struct app_entry_s *apps, *cur_app;
    struct sig_entry_s *sigs, *cur_sig;
    struct element_entry_s *eles, *cur_ele;
	uint cate_offset[MAX_CATEGORY_ID] = {0};
    uint sig_offset = 0;
    uint ele_offset = 0;
    uint flag = 0;

	struct application_s *app = NULL;
    struct sig_s *sig = NULL;
    struct element_s *element = NULL;

    apps = (struct app_entry_s *)malloc(
            sizeof(struct app_entry_s)*app_summary.app_nums);

    sigs = (struct sig_entry_s *)malloc(
            sizeof(struct sig_entry_s)*app_summary.sig_nums);

    eles = (struct element_entry_s *)malloc(
            sizeof(struct element_entry_s)*app_summary.ele_nums);

    if (apps == NULL || sigs == NULL || eles == NULL) {
        printf("install application failed\n");
        exit(1);
    }

    app_array.apps = apps;
    app_array.sigs = sigs;
    app_array.eles = eles;

#ifdef TEST_AC
	STAILQ_INIT(&app_list);
    /* TODO: we should free app_list in this function */
    init_http_sig(&app_list);
#endif

    STAILQ_FOREACH(app, root, next) {
        cur_app = apps + app_summary.cate_index[app->category] +
            cate_offset[app->category];
        cur_app->appid = app->id;
        cur_app->sig_index = sig_offset;
        STAILQ_FOREACH(sig, app->sigs, next) {
            cur_sig = sigs + sig_offset;
            /* TODO: appCD + appID? */
            cur_sig->sigid = ((app->category & 0xFFFF) << 16) |
                (cur_app->appid & 0xFFFF);
            cur_sig->proto = sig->proto;
            cur_sig->enable = sig->enable;
            cur_sig->priority = sig->priority;
            /* TODO: how to deal with http_method? */
            cur_sig->F_ele_index = ele_offset;
            if (sig->ele_num > 1)
                flag = 1;

            STAILQ_FOREACH(element, &(sig->elements), next) {
                /* update element id */
                element->id = ((element->id & 0xFF)<<24) | 
                    ele_offset;
#ifdef TEST_AC
                struct app_entry_t *sig_entry = 
                    new_app_sig(element->id, element->match);
                STAILQ_INSERT_TAIL(&app_list, sig_entry, next);
#endif
                cur_ele = eles + ele_offset;
                cur_ele->sigindex = sig_offset;
                cur_ele->flag = flag;
                cur_ele->exp_ele_index = ++ele_offset;
            }
            if (sig->ele_num != 0)
                cur_ele->exp_ele_index = 0xFFFF; 

            sig_offset++;
		}
        cur_app->sig_num = sig_offset - cur_app->sig_index;
        cate_offset[app->category]++;
	}

	return 0;
}

void dump_application_array()
{
    struct app_entry_s *apps, *cur_app;
    struct sig_entry_s *sigs, *cur_sig;
    struct element_entry_s *eles, *cur_ele;
    int i, j, k;

    apps = app_array.apps;
    sigs = app_array.sigs;
    eles = app_array.eles;

    printf("total app nums: %d\n", app_summary.app_nums);
    printf("total sig nums: %d\n", app_summary.sig_nums);
    printf("total ele nums: %d\n", app_summary.ele_nums);
    for (i = 0; i < MAX_CATEGORY_ID; i++) {
        printf("\tcate %d: index %d nums %d\n",
                i, app_summary.cate_index[i], app_summary.cate_num[i]);
        cur_app = apps + app_summary.cate_index[i];
        for (j = 0; j <  app_summary.cate_num[i]; j++) {
            cur_app += j;
            printf("\t\tapp: id %d\n", cur_app->appid);
            cur_sig = sigs + cur_app->sig_index;
            for (k = 0; k <  cur_app->sig_num; k++) {
                cur_sig += k;
                printf("\t\t\tsig: id %x proto %d enable %d pro %d\n",
                        cur_sig->sigid, cur_sig->proto, cur_sig->enable, cur_sig->priority);

                cur_ele = eles + cur_sig->F_ele_index;
                do {
                    printf("\t\t\t\tele: sigindex %d flag %d exp %d\n",
                            cur_ele->sigindex, cur_ele->flag, cur_ele->exp_ele_index);
                } while((cur_ele++)->exp_ele_index != 0xFFFF);
            }
        }
    }

    return;
}

int load_sig(const char *file)
{
    loadApp(file);
    dump_applications(app_root);
    get_Application_summary(app_root);
    install_Application(app_root);
    dump_application_array();

#ifdef TEST_AC
    /* TODO: get pattern list when parse_http_sig_ */
    sig_parse_adapter_ac(&app_list);
#endif

#ifdef TEST_REGEX
	appgraph = parseApplication(app_root);
    //graph_dump(appgraph);
#endif
	return 0;
}

