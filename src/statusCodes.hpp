#pragma once

#include "utility.hpp"

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

	std::map<int, std::string> statCode;

	StatusCode() {
		statCode[None] = "None";
		statCode[Incomplete] = "Incomplete";
		statCode[Continue] = "Continue";
		statCode[SwitchingProtocols] = "Switching Protocols";
		statCode[Processing] = "Processing";
		statCode[EarlyHints] = "EarlyHints";
		statCode[Ok] = "OK";
		statCode[Created] = "Created";
		statCode[Accepted] = "Accepted";
		statCode[NonAuthoritativeInformation] = "Non-Authoritative Information";
		statCode[NoContent] = "No Content";
		statCode[ResetContent] = "Reset Content";
		statCode[PartialContent] = "Partial Content";
		statCode[MultiStatus] = "Multi Status";
		statCode[AlreadyReported] = "AlreadyReported";
		statCode[IMUsed] = "IMUsed";
		statCode[MultipleChoices] = "Multiple Choices";
		statCode[MovedPermanently] = "Moved Permanently";
		statCode[Found] = "Found";
		statCode[SeeOther] = "See Other";
		statCode[NotModified] = "Not Modified";
		statCode[UseProxy] = "Use Proxy";
		statCode[TemporaryRedirect] = "Temporary Redirect";
		statCode[PermanentRedirect] = "Permanent Redirect";
		statCode[BadRequest] = "Bad Request";
		statCode[Unauthorized] = "Unauthorized";
		statCode[PaymentRequired] = "Payment Required";
		statCode[Forbidden] = "Forbidden";
		statCode[NotFound] = "Not Found";
		statCode[MethodNotAllowed] = "Method Not Allowed";
		statCode[NotAcceptable] = "Not Acceptable";
		statCode[ProxyAuthenticationRequired] = "Proxy Authentication Required";
		statCode[RequestTimeout] = "Request Time-out";
		statCode[Conflict] = "Conflict";
		statCode[Gone] = "Gone";
		statCode[LengthRequired] = "Length Required";
		statCode[PreconditionFailed] = "Precondition Failed";
		statCode[PayloadTooLarge] = "Request Entity Too Large";
		statCode[URITooLong] = "Request-URI Too Large";
		statCode[UnsupportedMediaType] = "Unsupported Media Type";
		statCode[RangeNotSatisfiable] = "Requested range not satisfiable";
		statCode[ExpectationFailed] = "Expectation Failed";
		statCode[Teapot] = "Teapot";
		statCode[MisdirectedRequest] = "Misdirected Request";
		statCode[UnprocessableEntity] = "Unprocessable Entity";
		statCode[Locked] = "Locked";
		statCode[FailedDependency] = "Failed Dependency";
		statCode[TooEarly] = "Too Early";
		statCode[UpgradeRequired] = "Upgrade Required";
		statCode[PreconditionRequired] = "Precondition Required";
		statCode[TooManyRequests] = "Too Many Requests";
		statCode[RequestHeaderFieldsTooLarge] = "Request Header Fields too Large";
		statCode[UnavailableForLegalReasons] = "Unavailable for Legal Reason";
		statCode[InternalServerError] = "Internal Server Error";
		statCode[NotImplemented] = "Not Implemented";
		statCode[BadGateway] = "Bad Gateway";
		statCode[ServiceUnavailable] = "Service Unavailable";
		statCode[GatewayTimeout] = "Gateway Time-out";
		statCode[HTTPVersionNotSupported] = "HTTP Version not supported";
		statCode[VariantAlsoNegotiates] = "Variant also negotiates";
		statCode[InsufficientStorage] = "Insufficient Storage";
		statCode[LoopDetected] = "Loop Detected";
		statCode[NotExtended] = "Not extended";
		statCode[NetworkAuthenticationRequired] = "Network Authentication Required";
	}

	std::string getError(int error) const { return (statCode.find(error)->second); }

	~StatusCode() {}
};

const StatusCode statusCode;
