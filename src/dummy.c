/* dummy symbols to keep superfluous CRAN checks happy
   (this package uses NAMESPACE C-level symbol registration
   but the checks don't get that) */

/* wasm doesn't like this so we skip the fake there */
#if !( defined(__EMSCRIPTEN__) || defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__) )

extern void R_registerRoutines(void);
extern void R_useDynamicSymbols(void);

void dummy(void) {
    R_registerRoutines();
    R_useDynamicSymbols();
}

#endif
