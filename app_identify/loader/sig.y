%{
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "application.h"
#include "key_pair.h"

void yyerror(char *s)
{
    printf("ERROR: %s\n", s);
}

extern int yylex();

extern struct application_list* app_root;
extern int parse_http_sig_(struct element_list *list, char *sig, int type);

%}

%union {
    char* ptr;
    struct application_s* app;
    struct sig_s* sig;
    struct sig_list* sigs;
    struct key_pair_s* pair;
    struct key_pair_list* pairs;
};

%token <ptr> TOKEN_SYMBOL
%token <ptr> TOKEN_VALUE
%token TOKEN_COMMA TOKEN_LBRANCH TOKEN_RBRANCH TOKEN_APPLICATION TOKEN_SIGNATURE

%type <app> application;
%type <sig> app_sig;
%type <sigs> app_sig_list;
%type <sigs> app_sig_node;
%type <pair> key_value_pair;
%type <pairs> key_value_list;

%%
applications:
    applications application
    {
        STAILQ_INSERT_TAIL(app_root, $2, next);
    }
    | application
    {
        app_root = create_application_list();
        STAILQ_INSERT_TAIL(app_root, $1, next);
    }
    ;

application:
    TOKEN_APPLICATION TOKEN_LBRANCH key_value_list app_sig_node TOKEN_RBRANCH
    {
        struct application_s *app = create_application();
        char *name = get_str_value_from_key($3, "appname");
        app->id = get_int_value_from_key($3, "appid");
        app->category = get_int_value_from_key($3, "category");
        assert(strlen(name) < MAX_APP_LENGTH);
        strcpy(app->name, name);
        app->sigs = $4;
       
        $$ = app;

        free_pair_list($3);
    }

app_sig_node:
    TOKEN_SIGNATURE	TOKEN_LBRANCH app_sig_list TOKEN_RBRANCH
    {
        $$ = $3;
    }
    ;

app_sig_list:
    app_sig_list app_sig 
    {
        STAILQ_INSERT_TAIL($1, $2, next);
        $$ = $1;
    }
    | app_sig 
    {
        struct sig_list *head =  create_sig_list();
        STAILQ_INSERT_TAIL(head, $1, next);
        $$ = head;
    }
 
app_sig:
    TOKEN_SYMBOL TOKEN_LBRANCH key_value_list TOKEN_RBRANCH
    {
        struct sig_s *sig = create_sig();
        char *match = get_str_value_from_key($3, "sig_match");
        sig->id = get_int_value_from_key($3, "sig_id");
        sig->proto = get_sigProto_value($3);
        sig->type = get_matchType_value($3);
        sig->enable = get_int_value_from_key($3, "sig_enable");
        sig->priority = get_int_value_from_key($3, "sig_priority");
        sig->dir = get_int_value_from_key($3, "sig_dir");
        assert(strlen(match) < MAX_SIG_LENGTH);
        strcpy(sig->match, match);
        sig->ele_num = parse_http_sig_(&(sig->elements), match, sig->type);

        $$ = sig;

        free_pair_list($3);
    }
    ;

key_value_list:
    key_value_list key_value_pair
    {
        STAILQ_INSERT_TAIL($1, $2, next);
        $$ = $1;
    }
    | key_value_pair
    {
        struct key_pair_list *head = create_pair_list();
        STAILQ_INSERT_TAIL(head, $1, next);
        $$ = head;
    }
    ;

key_value_pair:
    TOKEN_SYMBOL TOKEN_VALUE TOKEN_COMMA
    {
        $$ = create_key_value_pair($1, $2);
    }
    ;

%%

