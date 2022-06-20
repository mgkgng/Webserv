#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>

// https://www.restapitutorial.com/httpstatuscodes.html
enum StatusCodes {

	None = 0, Incomplete = 1,
	// Starting with 100, we have the informational status codes
	Continue = 100, SwitchingProtocols, Processing, EarlyHints,
	// Starting with 200, we have the successfull status codes
	Ok = 200, Created, Accepted, NonAuthoritativeInformation, NoContent,
	ResetContent, PartialContent, MultiStatus, AlreadyReported,
	IMUsed = 226,
	// Starting with 300, we have the redirection status codes
	MultipleChoices = 300, MovedPermanently, Found, SeeOther, NotModified, UseProxy,
	TemporaryRedirect = 307, PermanentRedirect,
	// Starting with 400, we have the clients error status codes
	BadRequest = 400, Unauthorized, PaymentRequired, Forbidden, NotFound,
	MethodNotAllowed, NotAcceptable, ProxyAuthenticationRequired, RequestTimeout,
	Conflict, Gone, LengthRequired, PreconditionFailed, PayloadTooLarge,
	URITooLong, UnsupportedMediaType, RangeNotSatisfiable, ExpectationFailed, Teapot,
	MisdirectedRequest = 421, UnprocessableEntity, Locked, FailedDependency, TooEarly, UpgradeRequired,
	PreconditionRequired = 428, TooManyRequests,
	RequestHeaderFieldsTooLarge = 431,
	UnavailableForLegalReasons = 451,
	// Starting with 500, we have the server errors status codes 
	InternalServerError = 500,NotImplemented, BadGateway, ServiceUnavailable,
	GatewayTimeout, HTTPVersionNotSupported, VariantAlsoNegotiates, InsufficientStorage, LoopDetected,
	NotExtended = 510, NetworkAuthenticationRequired
};

struct StatusCode {

	std::map<int, std::string> status_code;

	StatusCode() {
		status_code[None] = "None";
		status_code[Incomplete] = "Incomplete";
		status_code[Continue] = "Continue";
		status_code[SwitchingProtocols] = "Switching Protocols";
		status_code[Processing] = "Processing";
		status_code[EarlyHints] = "EarlyHints";
		status_code[Ok] = "OK";
		status_code[Created] = "Created";
		status_code[Accepted] = "Accepted";
		status_code[NonAuthoritativeInformation] = "Non-Authoritative Information";
		status_code[NoContent] = "No Content";
		status_code[ResetContent] = "Reset Content";
		status_code[PartialContent] = "Partial Content";
		status_code[MultiStatus] = "Multi Status";
		status_code[AlreadyReported] = "AlreadyReported";
		status_code[IMUsed] = "IMUsed";
		status_code[MultipleChoices] = "Multiple Choices";
		status_code[MovedPermanently] = "Moved Permanently";
		status_code[Found] = "Found";
		status_code[SeeOther] = "See Other";
		status_code[NotModified] = "Not Modified";
		status_code[UseProxy] = "Use Proxy";
		status_code[TemporaryRedirect] = "Temporary Redirect";
		status_code[PermanentRedirect] = "Permanent Redirect";
		status_code[BadRequest] = "Bad Request";
		status_code[Unauthorized] = "Unauthorized";
		status_code[PaymentRequired] = "Payment Required";
		status_code[Forbidden] = "Forbidden";
		status_code[NotFound] = "Not Found";
		status_code[MethodNotAllowed] = "Method Not Allowed";
		status_code[NotAcceptable] = "Not Acceptable";
		status_code[ProxyAuthenticationRequired] = "Proxy Authentication Required";
		status_code[RequestTimeout] = "Request Time-out";
		status_code[Conflict] = "Conflict";
		status_code[Gone] = "Gone";
		status_code[LengthRequired] = "Length Required";
		status_code[PreconditionFailed] = "Precondition Failed";
		status_code[PayloadTooLarge] = "Request Entity Too Large";
		status_code[URITooLong] = "Request-URI Too Large";
		status_code[UnsupportedMediaType] = "Unsupported Media Type";
		status_code[RangeNotSatisfiable] = "Requested range not satisfiable";
		status_code[ExpectationFailed] = "Expectation Failed";
		status_code[Teapot] = "Teapot";
		status_code[MisdirectedRequest] = "Misdirected Request";
		status_code[UnprocessableEntity] = "Unprocessable Entity";
		status_code[Locked] = "Locked";
		status_code[FailedDependency] = "Failed Dependency";
		status_code[TooEarly] = "Too Early";
		status_code[UpgradeRequired] = "Upgrade Required";
		status_code[PreconditionRequired] = "Precondition Required";
		status_code[TooManyRequests] = "Too Many Requests";
		status_code[RequestHeaderFieldsTooLarge] = "Request Header Fields too Large";
		status_code[UnavailableForLegalReasons] = "Unavailable for Legal Reason";
		status_code[InternalServerError] = "Internal Server Error";
		status_code[NotImplemented] = "Not Implemented";
		status_code[BadGateway] = "Bad Gateway";
		status_code[ServiceUnavailable] = "Service Unavailable";
		status_code[GatewayTimeout] = "Gateway Time-out";
		status_code[HTTPVersionNotSupported] = "HTTP Version not supported";
		status_code[VariantAlsoNegotiates] = "Variant also negotiates";
		status_code[InsufficientStorage] = "Insufficient Storage";
		status_code[LoopDetected] = "Loop Detected";
		status_code[NotExtended] = "Not extended";
		status_code[NetworkAuthenticationRequired] = "Network Authentication Required";
	}

	std::string getError(int error) const { return (status_code.find(error)->second); }

	~StatusCode() {}
};

const StatusCode statusCode;

// http://www.cplusplus.com/articles/D9j2Nwbp/
template<typename T>
std::string to_string(const T &status_code)
{  
	std::ostringstream in_string;
	in_string << status_code;
	return (in_string.str());
}

std::string statusCodeToString(int status_code)
{
	return (to_string(status_code) + " " + statusCode.getError(status_code));
}
