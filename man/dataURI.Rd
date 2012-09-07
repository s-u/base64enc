\name{dataURI}
\alias{dataURI}
\title{
Create a data URI string
}
\description{
  \code{dataURI} creates URI with the \code{data:} scheme by encoding
  the payload either using base64 ot URI encoding.
}
\usage{
dataURI(data, mime = "", encoding = "base64", file)
}
\arguments{
  \item{data}{raw vector, connection or character vector to use as
  payload. Character vectors of more than one element are collapsed
  using \code{"\n"} before encoding.}
  \item{mime}{MIME-type of the data (per standard "" is interpreted as
  "text/plain;charset=US-ASCII" without including it in the URI)}
  \item{encoding}{data encoding to use. Must be either \code{"base64"}
  or \code{NULL}}
  \item{file}{filename (string) to open as payload. \code{file} and
  \code{data} are mutually exclusive}
}
%\details{
%}
\value{
  string of the form \code{data:[mime][;base64],<encoded-payload>}
}
\references{
\href{http://tools.ietf.org/html/rfc2397}{RFC 2397 The "data" URL scheme}
}
\author{
Simon Urbanek
}
%\note{
%}

\examples{
  dataURI(as.raw(1:10)) # default is base64
  dataURI(as.raw(1:10), encoding=NULL) # URI
  if (require("png", quietly=TRUE)) {
    # let's say you have an image - e.g. from dev.capture(TRUE)
    img <- matrix(1:16/16, 4)
    dataURI(writePNG(img), "image/png")
    # or straight from a file
    dataURI(file=system.file("img", "Rlogo.png", package="png"), mime="image/png")
  }
}
\keyword{manip}
