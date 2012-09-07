\name{dataURI}
\alias{dataURI}
\title{
Create a data: URI string
}
\description{
  \code{dataURI} creates URI with the \code{data:} schema by encoding
  the payload either using base64 ot URI encoding.
}
\usage{
dataURI(data, mime = "", encoding = "base64")
}
\arguments{
  \item{data}{raw vector or a string to use as payload}
  \item{mime}{MIME-type of the data (defaults to "text/plain")}
  \item{encoding}{data encoding to use. Mut be either \code{"base64"}
  or \code{NULL}}
}
%\details{
%}
\value{
  string of the form \code{data:[mime][;base64],<encoded-payload>}
}
%\references{
%}
\author{
Simon Urbanek
}
%\note{
%}

\examples{
  dataURI(as.raw(1:10)) # default is base64
  dataURI(as.raw(1:10), encoding=NULL) # URI
}
\keyword{manip}
