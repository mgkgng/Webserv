#include "HandleCode.hpp"

#include <algorithm>

HandleCode::HandleCode(const HandleCode & handlecode) {
	this->code = handlecode.getCode();
	this->route = handlecode.getRoute();
	this->responsecode = handlecode.getResponseCode();
}

HandleCode::HandleCode(int code, Route route, int responsecode) {
	//* apparently here we should handle invalid http codes
	/*std::vector<const unsigned int>::iterator valid = std::find(validHTTPCodes.begin(), Webserv::validHTTPCodes.end(), code);
	if (valid == validHTTPCodes.end()) {
		throw InvalidHTTPCode();
	}
	valid = std::find(validHTTPCodes.begin(), validHTTPCodes.end(), responsecode);
	if (valid == validHTTPCodes.end() && responsecode != 0) {
		throw InvalidHTTPCode();
	}*/
	this->code = code;
	this->route = route;
	this->responsecode = responsecode;
}

HandleCode::~HandleCode() { }

HandleCode &  HandleCode::operator=(const HandleCode & handlecode) {
	this->code = handlecode.getCode();
	this->route = handlecode.getRoute();
	this->responsecode = handlecode.getResponseCode();
	return (*this);
}

unsigned int	HandleCode::getCode() const {
	return (this->code);
}

Route	HandleCode::getRoute() const {
	return (this->route);
}

unsigned int	HandleCode::getResponseCode() const {
	return (this->responsecode);
}
