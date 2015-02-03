#include <stdio.h>

#include <Rinternals.h>

#define report(reason) { snprintf(cause, sizeof(cause), "INVALID byte 0x%02x at 0x%lx (%lu, line %lu): %s\n", (int) buf[i], i, i, line, reason); if (max_cl) *max_cl = maxcl; return 1; }

static char cause[512];

static int utf8_check_(const unsigned char *buf, unsigned long len, int *max_cl, int min_char) {
    unsigned long i = 0, bp = len, line = 1;
    int maxcl = 1;
    
    while (i < bp) {
	if (min_char > 0 && buf[i] < min_char)
	    report("disallowed control character");
	if (buf[i] < 128) {
	    if (buf[i] == '\n') line++;
	} else if (buf[i] < 192) {
	    report("2+ byte of a sequence found in first position");
	} else if (buf[i] < 194) {
	    report("overlong encoding (<=127 encoded)");
	} else if (buf[i] < 224) { /* 2-byte seq */
	    if (i + 1 < bp) {
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid second byte in 2-byte encoding");
		}
		if (maxcl < 2) maxcl = 2;
	    } else break;
	} else if (buf[i] < 240) { /* 3-byte seq */
	    if (i + 2 < bp) {
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid second byte in 3-byte encoding");
		}
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid third byte in 3-byte encoding");
		}
		if (maxcl < 3) maxcl = 3;
	    } else break;
	} else if (buf[i] < 245) { /* 4-byte seq */
	    if (i + 3 < bp) {
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid second byte in 4-byte encoding");
		}
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid third byte in 4-byte encoding");
		}
		i++;
		if (buf[i] < 0x80 || buf[i] > 0xbf) {
		    report("invalid fourth byte in 4-byte encoding");
		}
		if (maxcl < 3) maxcl = 3;
	    } else break;
	} else if (buf[i] < 254) {
	    report("invalid start of a codepoint above 0x10FFFF");
	} else {
	    report("invalid start byte (FE/FF)");
	}
	i++;
    }
    bp -= i;
    if (bp > 0)
	report("unterminated multi-byte sequence at the end of file");
    return 0;
}

SEXP utf8_check(SEXP sWhat, SEXP sQuiet, SEXP sXLen, SEXP sMinChar) {
    if (TYPEOF(sWhat) != RAWSXP) Rf_error("invalid input");
    {
	int maxcl = 0;
	int res = utf8_check_((const unsigned char*) RAW(sWhat), XLENGTH(sWhat), &maxcl, asInteger(sMinChar));
	
	if (asInteger(sQuiet) == 0 && res)
	    Rf_error("%s", cause);
	if (asInteger(sXLen) != 0)
	    return ScalarInteger((res == 0) ? maxcl : (-maxcl));
	return ScalarLogical((res == 0) ? TRUE : FALSE);
    }
}
