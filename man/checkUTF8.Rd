\name{checkUTF8}
\alias{checkUTF8}
\title{
  Check the validity of a byte stream ot be interpreted as UTF8.
}
\description{
  \code{checkUTF8} check whether a given raw vector can be used as a
  valid string encoded in UTF8.
}
\usage{
checkUTF8(what, quiet = FALSE,	charlen = FALSE, min.char = 1L)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{what}{raw vector with the payload}
  \item{quiet}{logical, if \code{TRUE} then the function will not fail
  but report success/failure via its result, otherwise failures are
  considered errors.}
  \item{charlen}{logical, if \code{TRUE} then the function returns the
  length of the longest byte sequence representing a character in the
  file.}
  \item{min.char}{integer, any bytes below this value are considered
  control chacters and reported as errors. The default value of 1L
  guards against strings including NULs.}
}
\value{
  If \code{charlen=FALSE}: \code{TRUE} on success, \code{FALSE} if the
  payload is invalid and \code{quite=TRUE}. 

  If \code{charlen=TRUE}: positive integer corresponding to the
  longest encoded sequence on success, negative integer on failure.
}
\author{
Simon Urbanek
}
%\examples{
%}
\keyword{manip}
