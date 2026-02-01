/* base64.c - encoding/decoding of base64

   (C)Copyright 2011,12 Simon Urbanek

   Licensed under a choice of GPLv2 or GPLv3

*/

#include <stddef.h>

/* it should be something like R_xlen_t -- must be signed, though! */
typedef ptrdiff_t blen_t;

#include <string.h>
#include <Rinternals.h>

#include <Rversion.h>
/* for compatibility with older R versions */
#if R_VERSION < R_Version(3,0,0)
#define XLENGTH(X) LENGTH(X)
#endif

/* -- base64 encode/decode -- */

static char *base64encode(const unsigned char *src, blen_t len, char *dst);
static blen_t base64decode(const char *src, void *dst, blen_t max_len);

static const char *b64tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define SRC(i) ((i < len) ? src[i] : 0) /* guarded access to src[] */

/* dst must be at least (len + 2) / 3 * 4 + 1 bytes long and will be NUL terminated when done */
static char *base64encode(const unsigned char *src, blen_t len, char *dst) {
    while (len >= 3) { /* no need to worry about padding - faster */
	*(dst++) = b64tab[src[0] >> 2];
	*(dst++) = b64tab[((src[0] & 0x03) << 4) | ((src[1] & 0xf0) >> 4)];
	*(dst++) = b64tab[((src[1] & 0x0f) << 2) | ((src[2] & 0xc0) >> 6)];
	*(dst++) = b64tab[src[2] & 0x3f];
	src += 3;
	len -= 3;
    }
    if (len > 0) { /* last chunk - may need padding and guarding against OOB */
	*(dst++) = b64tab[src[0] >> 2];
	*(dst++) = b64tab[((src[0] & 0x03) << 4) | ((SRC(1) & 0xf0) >> 4)];
	*(dst++) = (len > 1) ? b64tab[((src[1] & 0x0f) << 2) | ((SRC(2) & 0xc0) >> 6)] : '=';
	*(dst++) = (len > 2) ? b64tab[src[2] & 0x3f] : '=';
    }
    *dst = 0;
    return dst;
}

#undef SRC

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
static blen_t base64decode(const char *src, void *dst, blen_t max_len) {
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
    return (blen_t) (t - (unsigned char*) dst);
}

static char stb[8192];

SEXP B64_encode(SEXP what, SEXP linewidth, SEXP newline) {
    const char *nl = 0;
    char *buf = stb;
    const unsigned char *src = (const unsigned char*) RAW(what);
    blen_t buflen = sizeof(stb), slice;
    blen_t lwd = 0, len = XLENGTH(what), step;
    if (len == 0) return allocVector(STRSXP, 0);
    if (TYPEOF(newline) == STRSXP && LENGTH(newline) > 0)
	nl = CHAR(STRING_ELT(newline, 0));
    if (TYPEOF(linewidth) == INTSXP)
	lwd = asInteger(linewidth);
    if (TYPEOF(linewidth) == REALSXP)
	lwd = (blen_t) asReal(linewidth);
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
	if (lwd == 0 || len <= step) { /* easy, just call encode and out */
	    base64encode(src, len, buf);
	    return mkString(buf);
	}
	/* one string but with NLs */
	{
	    char *dst = buf;
	    while (len) {
		blen_t amt = (len > step) ? step : len;
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
	blen_t i = 0;
	SEXP res = PROTECT(allocVector(STRSXP, len / step + 1));
	slice = lwd + 1;
	if (slice > buflen) {
	    buf = R_alloc(4, (slice >> 2) + 1);
	    buflen = slice;
	}
	while(len) {
	    blen_t amt = (len > step) ? step : len;
	    base64encode(src, amt, buf);
	    src += amt;
	    SET_STRING_ELT(res, i++, mkChar(buf));
	    len -= amt;
	}
#if R_VERSION >= R_Version(4,5,0)
	if (i < XLENGTH(res)) { /* cannot shorten, copy */
	    SEXP nres = PROTECT(allocVector(STRSXP, i));
	    blen_t j = 0;
	    while (j < i) {
		SET_STRING_ELT(nres, j, STRING_ELT(res, j));
		j++;
	    }
	    res = nres;
	    UNPROTECT(1); /* in theory we should swap protection,
			     but we return directly anyway */
	}
#else
	SETLENGTH(res, i);
#endif
	UNPROTECT(1);
	return res;
    }
}

SEXP B64_decode(SEXP what) {
    /* we need to allocate enough space to decode.
       FIXME: For now, we assume it's full of payload;
       we will over-allocate if there is junk behind it */
    blen_t tl = 0;
    SEXP res;
    blen_t ns = XLENGTH(what), i;
    unsigned char *dst;
    if (TYPEOF(what) != STRSXP && TYPEOF(what) != RAWSXP)
	Rf_error("I can only decode base64 strings or raw vectors");
    if (TYPEOF(what) == RAWSXP) {
	tl = XLENGTH(what);
    } else {
	for (i = 0; i < ns; i++)
	    tl += strlen(CHAR(STRING_ELT(what, i)));
    }
    tl = (tl / 4) * 3 + 4;
    res = PROTECT(allocVector(RAWSXP, tl));
    dst = (unsigned char*) RAW(res);
    if (TYPEOF(what) == RAWSXP) {
	blen_t al = base64decode((const char*)RAW(what), dst, tl);
	if (al < 0) /* this should never happen as we allocated enough space ... */
	    Rf_error("decoding error - insufficient buffer space");
	dst += al;
    } else
	for (i = 0; i < ns; i++) {
	    blen_t al = base64decode(CHAR(STRING_ELT(what, i)), dst, tl);
	    if (al < 0) /* this should never happen as we allocated enough space ... */
		Rf_error("decoding error - insufficient buffer space");
	    tl -= al;
	    dst += al;
	}
    blen_t rl = (blen_t) (dst - ((unsigned char*) RAW(res)));
    if (rl < XLENGTH(res)) {
#if R_VERSION >= R_Version(4,5,0)
      SEXP nres = allocVector(RAWSXP, rl);
      memcpy(RAW(nres), RAW(res), rl);
      res = nres;
#else
      SETLENGTH(res, dst - ((unsigned char*) RAW(res)));
#endif
    }
    UNPROTECT(1);
    return res;
}
