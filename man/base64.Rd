\name{base64}
\alias{base64}
\alias{base64encode}
\alias{base64decode}
\title{
  Encode/decode data into/from base64 encoding
}
\description{
  \code{base64encode} encodes a data into base64 encoding. The source
  can be a file, binary connection or a raw vector.

  \code{base64decode} decodes a base64-encoded string into binary
  data. The source can be a string or a connection, the output is
  either a raw vector (\code{output=NULL}) or a binary connection.
}
\usage{
base64encode(what, linewidth, newline)
base64decode(what, output = NULL, file)
}
\arguments{
  \item{what}{data to be encoded/decoded. For \code{base64encode} it
  can be a raw vector, text connection or file name. For
  \code{base64decode} it can be a string or a binary connection.}
  \item{linewidth}{if set, the output is split into lines with at most
    \code{linewidth} characters per line. Zero or \code{NA} denotes no
    limit and values 1 .. 3 are silently treated as 4 since that is the
    shortest valid line.}
  \item{newline}{only applicable if \code{linewidth} is set; if set
    (string), the result will be a single string with all lines
    joined using the \code{newline} string}
  \item{output}{if \code{NULL} then the output will be a raw vector
    with the decoded data, otherwise it must be either a filename
    (string) or a binary connection.}
  \item{file}{file name (string) for data to use as input instead of
    \code{what}. It is essentially just a shorthand for
    \code{base64decode(file(name))}. Only one of \code{what} and
    \code{file} can be specified.}
}
%\details{
%}
\value{
  \code{base64encode}: A character vector. If \code{linewith > 0} and
  \code{newline} is not set then it will consist of as many elements
  as there are lines. Otherwise it is a single string.

  \code{base64decode}: If \code{output = NULL} then a raw vector with
  the decoded content, otherwise the number of bytes written into the
  connection.
}
%\references{
%}
\author{
  Simon Urbanek
}
%\note{
%}
%\seealso{
%}
\examples{
  base64encode(1:100)
  base64encode(1:100, 70)
  base64encode(1:100, 70, "\n")
  x <- charToRaw("the decoded content, otherwise the number of bytes")
  y <- base64decode(base64encode(x))
  stopifnot(identical(x, y))
}
\keyword{manip}
