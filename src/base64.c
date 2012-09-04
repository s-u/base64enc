/* base64.c - encoding/decoding of base64

   (C)Copyright 2011,12 Simon Urbanek

   Licensed under a choice of GPLv2 or GPLv3

*/

/* int for now but it should be something like R_xlen_t */
#define blen_t int

/* -- base64 encode/decode -- */

static char *base64encode(const unsigned char *src, int len, char *dst);
static int base64decode(const char *src, void *dst, int max_len);

static const char *b64tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* dst must be at least (len + 2) / 3 * 4 + 1 bytes long and will be NUL terminated when done */
static char *base64encode(const unsigned char *src, int len, char *dst) {
    while (len > 0) {
	*(dst++) = b64tab[src[0] >> 2];
	*(dst++) = b64tab[((src[0] & 0x03) << 4) | ((src[1] & 0xf0) >> 4)];
	*(dst++) = (len > 1) ? b64tab[((src[1] & 0x0f) << 2) | ((src[2] & 0xc0) >> 6)] : '=';
	*(dst++) = (len > 2) ? b64tab[src[2] & 0x3f] : '=';
	src += 3;
	len -= 3;
    }
    *dst = 0;
    return dst;
}

static unsigned int val(const char **src) {
    while (1) {
	char c = **src;
	if (c) src[0]++; else return 0x10000;
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 52;
	if (c == '+') return 62;
	if (c == '/') return 63;
	if (c == '=')
	    return 0x10000;
	/* we loop as to skip any blanks, newlines etc. */
    }
}

/* returns the decoded length or -1 if max_len was not enough */
static int base64decode(const char *src, void *dst, int max_len) {
    unsigned char *t = (unsigned char*) dst, *end = t + max_len;
    while (*src && t < end) {
	unsigned int v = val(&src);
	if (v > 64) break;
	*t = v << 2;
	v = val(&src);
	*t |= v >> 4;
	if (v < 64) {
	    if (++t == end) return -1;
	    *t = v << 4;
	    v = val(&src);
	    *t |= v >> 2;
	    if (v < 64) {
		if (++t == end) return -1;
		*t = v << 6;
		v = val(&src);
		*t |= v & 0x3f;
		if (v < 64) t++;
	    }
	}
    }
    return (int) (t - (unsigned char*) dst);
}

static char stb[8192];

#include <Rinternals.h>
#include <string.h>

SEXP B64_encode(SEXP what, SEXP linewidth, SEXP newline) {
    const char *nl = 0;
    char *buf = stb;
    const unsigned char *src = (const unsigned char*) RAW(what);
    blen_t buflen = sizeof(stb), slice;
    int lwd = 0, len = LENGTH(what), step;
    if (len == 0) return allocVector(STRSXP, 0);
    if (TYPEOF(newline) == STRSXP && LENGTH(newline) > 0)
	nl = CHAR(STRING_ELT(newline, 0));
    if (TYPEOF(linewidth) == INTSXP || TYPEOF(linewidth) == REALSXP)
	lwd = asInteger(linewidth);
    if (lwd <= 0) lwd = 0;
    else if (lwd < 4) lwd = 4; /* there must be at least 4 chars per line */
    lwd -= lwd & 3;
    step = lwd / 4 * 3;
    /* make sure we get big enough buffer for what we need to do */
    if (lwd == 0 || nl) {
	blen_t nll = nl ? strlen(nl) : 0;
	slice = (blen_t) len * 4 / 3 + 4;
	if (lwd && nll) slice += (slice / lwd + 1) * nll;
	if (slice > buflen) {
	    buf = R_alloc(256, (slice >> 8) + 1); /* making sure we can use at least 73 bits where possible */
	    buflen = slice;
	}
	if (lwd == 0 || len <= step) { /* easy, jsut call encode and out */
	    base64encode(src, len, buf);
	    return mkString(buf);
	}
	/* one string but with NLs */
	{
	    char *dst = buf;
	    while (len) {
		int amt = (len > step) ? step : len;
		dst = base64encode(src, amt, dst);
		src += amt;
		len -= amt;
		if (len) {
		    strcpy(dst, nl);
		    dst += nll;
		}
	    }
	    return mkString(buf);
	}
    } else { /* lwd and no nl = vector result */
	int i = 0;
	SEXP res = PROTECT(allocVector(STRSXP, len / step + 1));
	slice = lwd + 1;
	if (slice > buflen) {
	    buf = R_alloc(4, (slice >> 2) + 1);
	    buflen = slice;
	}
	while(len) {
	    int amt = (len > step) ? step : len;
	    base64encode(src, amt, buf);
	    src += amt;
	    SET_STRING_ELT(res, i++, mkChar(buf));
	    len -= amt;
	}
	if (i < LENGTH(res)) SETLENGTH(res, i);
	UNPROTECT(1);
	return res;
    }
}
