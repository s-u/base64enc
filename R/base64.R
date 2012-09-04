base64encode <- function(what, linewidth, newline) {
  if (missing(linewidth)) linewidth <- NA_integer_
  if (missing(newline)) newline <- NULL
  if (is.character(what)) what <- charToRaw(paste(what, collapse='\n'))
  .Call(B64_encode, as.raw(what), linewidth, newline)
}
