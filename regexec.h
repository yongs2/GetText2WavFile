#ifndef __REG_EXEC_H__
#define __REG_EXEC_H__

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <stdarg.h>

int RegExec(regex_t * r, const char * to_match, int nMaxGroup, va_list* pAp) ;
int RegComp(regex_t * r, const char * regex_text);
int ChkRegex(const char *regex_text, const char *find_text, int nMaxGroup, ...);

#endif  // __REG_EXEC_H__