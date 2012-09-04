\name{base64encode}
\alias{base64encode}
\title{
  Encode data into base64 encoding
}
\description{
  \code{base64encode} encodes a data into base64 encoding. The source
  can be a file, connection or a raw vector.
}
\usage{
base64encode(what, linewidth, newline)
}
\arguments{
  \item{what}{data to be encoded. It can be a raw vector, connection or
    file name}
  \item{linewidth}{if set, the output is split into lines with at most
    \code{linewidth} characters per line. Zero or \code{NA} denotes no
    limit and values 1 .. 3 are silently treated as 4 since that is the
    shortest valid line.}
  \item{newline}{only applicable if \code{linewidth} is set; if set
    (single string), the result will be a single string with all lines
    joined using the \code{newline} string}
}
%\details{
%}
\value{
  A character vector. If \code{linewith > 0} and \code{newline} is not
  set then it will consist of as many elements as there are
  lines. Otherwise it is a single string.
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
  base64encode(as.raw(1:100))
  base64encode(as.raw(1:100), 70)
  base64encode(as.raw(1:100), 70, "\n")
}
\keyword{manip}

