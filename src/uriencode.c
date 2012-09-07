#include <string.h>
#include <Rinternals.h>

static const char *plain = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
static const char *hex = "0123456789ABCDEF";

/* flexible and fast for long strings. Since short string are, well,
   short, the overhead of building a table should play no role */
SEXP C_URIencode(SEXP what, SEXP resrv) {
    SEXP res;
    char tab[256];
    const unsigned char *c = (const unsigned char*) plain;
    if (TYPEOF(what) != STRSXP && TYPEOF(what) != RAWSXP)
	Rf_error("input must be a raw or character vector");
    memset(tab, 0, sizeof(tab));
    while (*c) tab[*(c++)] = 1;
    if (TYPEOF(resrv) == STRSXP) {
	int n = LENGTH(resrv), i;
	for (i = 0; i < n; i++) {
	    c = (const unsigned char*) CHAR(STRING_ELT(resrv, i));
	    while (*c) tab[*(c++)] = 1;
	}
    }
    if (TYPEOF(what) == RAWSXP) {
	int len = 0;
	const unsigned char *cend =
	    (c = (const unsigned char*) RAW(what)) +
	    LENGTH(what);
	char *enc, *ce;
	while (c < cend)
	    len += tab[*(c++)] ? 1 : 3;
	ce = enc = (char*) R_alloc(1, len + 1);
	c = (const unsigned char*) RAW(what);
	while (c < cend)
	    if (tab[*c])
		*(ce++) = *(c++);
	    else {
		*(ce++) = '%';
		*(ce++) = hex[*c >> 4];
		*(ce++) = hex[*(c++) & 0x0F];
	    }
	*ce = 0;
	return mkString(enc);
    } else {
	int i, n = LENGTH(what), maxlen = 0;
	char *enc, *ce;
	res = allocVector(STRSXP, n);
	if (n == 0) return res;
	PROTECT(res);
	/* find the longest encoded string to allocate buffer */
	for (i = 0; i < n; i++) { /* FIXME: we should tanslate to UTF8 */
	    int len = 0;
	    c = (const unsigned char*) CHAR(STRING_ELT(what, i));
	    while (*c)
		len += tab[*(c++)] ? 1 : 3;
	    if (len > maxlen) maxlen = len;
	}
	enc = (char*) R_alloc(1, maxlen + 1);
	for (i = 0; i < n; i++) {
	    c = (const unsigned char*) CHAR(STRING_ELT(what, i));
	    ce = enc;
	    while (*c)
		if (tab[*c])
		    *(ce++) = *(c++);
		else {
		    *(ce++) = '%';
		    *(ce++) = hex[*c >> 4];
		    *(ce++) = hex[*(c++) & 0x0F];
		}
	    *ce = 0;
	    SET_STRING_ELT(res, i, mkChar(enc));
	}
	UNPROTECT(1);
	return res;
    }
}
