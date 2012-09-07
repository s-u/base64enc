dataURI <- function(data, mime="", encoding="base64") {
   if (!is.null(encoding) && !isTRUE(encoding == "base64")) stop('encoding must be either NULL or "base64"') 
   prefix <- paste("data:", as.character(mime)[1], if (!is.null(encoding)) ";base64", ",", sep ='')
   if (!is.raw(data)) data <- paste(as.character(data), collapse='\n')
   paste(prefix, if (is.null(encoding)) .Call(C_URIencode, data, NULL) else .Call(B64_encode, data, 0L, NULL), sep='')
}
