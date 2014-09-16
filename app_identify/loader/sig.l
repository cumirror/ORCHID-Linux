%{
/*
Bison-Flex 笔记
http://www.cnblogs.com/me115/archive/2010/10/27/1862180.html
二义性的说明
http://course.cug.edu.cn/bianyi/shiyan/CHAPTER/f1.htm
状态的说明
http://www.cppblog.com/woaidongmao/archive/2008/11/23/67637.html
中文手册
http://blog.csdn.net/sirouni2003/article/details/590661
酷壳上的文章
http://coolshell.cn/articles/1547.html
# Tips:
#    1.<*>代表任意状态
#    2.通过BEGIN来设置状态
#    3.
#
#
#
#

*/
#include "sig.tab.h"
#include <stdio.h>
#include <string.h>

#define lex_debug printf
extern YYSTYPE yylval;
%}

newline \n
lbranch	\{
rbranch	\}
comma ;
whitespace [ \t\n\r]+
values [^ \r\n\t\{\}]+
symbols [^ \n\t\{\};]+
comment	#.*\n
app	"application"

%%
<*>{whitespace} {
    /* do nothing */
};

<*>{comma} {
	return TOKEN_COMMA;
};

<*>{comment} {
	/* comment, do nothing */
};

{symbols} {
    if(strcasecmp(yytext, "application") == 0) {
        return TOKEN_APPLICATION;
    } else if(strcasecmp(yytext, "sign") == 0){
        return TOKEN_SIGNATURE;
    } else {
        yylval.ptr = strdup(yytext);
        return TOKEN_SYMBOL;
    }
};

<INITIAL>{lbranch} {
	BEGIN INITIAL;
	return TOKEN_LBRANCH;
};

{values} {
	if (yytext[yyleng-1] == ';') {
		yyless(yyleng - 1);
	}
    yylval.ptr = strdup(yytext);
	return TOKEN_VALUE;
};

{rbranch} {
	return TOKEN_RBRANCH;
};

%%

int yywrap()
{
    return 1;
}

