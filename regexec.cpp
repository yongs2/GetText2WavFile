#include "regexec.h"

int RegExec(regex_t * r, const char * to_match, int nMaxGroup, va_list* pAp) {
    /* "P" is a pointer into the string which points to the end of the previous match. */
    const char * p = to_match;
    /* "N_matches" is the maximum number of matches allowed. */
    const int n_matches = 10;
    /* "M" contains the matches found. */
    regmatch_t m[n_matches];

    while (1) {
        int nomatch =-1;

        nomatch = regexec(r, p, n_matches, m, 0);
        //printf (" regexec(to_match=%s, n_matches=%d)=%d\n", to_match, n_matches, nomatch);
        if (nomatch > 0) {
            //printf ("No more matches.\n");
            return 0;
        }
        //printf (" regexec(to_match=%s, n_matches=%d), r->re_nsub=%d\n", to_match, n_matches, r->re_nsub);
        for(int i = 1; i < r->re_nsub+1; ++i) {
            printf("match %d from index %d to %d: ", i, m[i].rm_so, m[i].rm_eo);
            for(int j = m[i].rm_so; j < m[i].rm_eo; ++j) {
                printf("%c", to_match[j]);
            }
            printf("\n");
            if(i <= nMaxGroup) {
                char *pszResult = va_arg(*pAp, char*);
                strncpy(pszResult, &to_match[m[i].rm_so], m[i].rm_eo - m[i].rm_so);
                pszResult[m[i].rm_eo - m[i].rm_so] = '\0';
                printf("Result[%d]=[%s]\n", i, pszResult);
            }
        }
        return r->re_nsub+1;
    }
    return 0;
}

#define MAX_ERROR_MSG 0x1000
int RegComp(regex_t * r, const char * regex_text) {
    int status = regcomp(r, regex_text, REG_EXTENDED | REG_NEWLINE);
    //printf(" compile_regex(%s)=%d, r->re_nsub=%d\n", regex_text, status, r->re_nsub);
    if (status != 0) {
        char error_message[MAX_ERROR_MSG];
        regerror(status, r, error_message, MAX_ERROR_MSG);
        printf("Regex error compiling '%s': %s\n", regex_text, error_message);
        return 1;
    }
    return 0;
}

int ChkRegex(const char *regex_text, const char *find_text, int nMaxGroup, ...) {
    int nRet = 0;
    regex_t r;

    //printf ("Trying to find '%s' in '%s'\n", regex_text, find_text);
    nRet = RegComp(&r, regex_text);
    if (nRet == 0) {
        va_list ap;
        va_start(ap, nMaxGroup);
        nRet = RegExec(&r, find_text, nMaxGroup,  &ap);
        va_end(ap);
    }
    regfree(&r);

    return nRet;
}