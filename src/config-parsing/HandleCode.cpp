#include <Webserv.hpp>

#include <algorithm>

Webserv::HandleCode::HandleCode(const Webserv::HandleCode & handlecode) {
	this->code = handlecode.getCode();
	this->route = handlecode.getRoute();
	this->responsecode = handlecode.getResponseCode();
}

Webserv::HandleCode::HandleCode(int code, Webserv::Route route, int responsecode) {
	std::vector<const unsigned int>::iterator valid = std::find(Webserv::validHTTPCodes.begin(), Webserv::validHTTPCodes.end(), code);
	if (valid == Webserv::validHTTPCodes.end()) {
		throw InvalidHTTPCode();
	}
	valid = std::find(Webserv::validHTTPCodes.begin(), Webserv::validHTTPCodes.end(), responsecode);
	if (valid == Webserv::validHTTPCodes.end() && responsecode != 0) {
		throw InvalidHTTPCode();
	}
	this->code = code;
	this->route = route;
	this->responsecode = responsecode;
}

Webserv::HandleCode::~HandleCode() {

}

Webserv::HandleCode &  Webserv::HandleCode::operator=(const Webserv::HandleCode & handlecode) {
	this->code = handlecode.getCode();
	this->route = handlecode.getRoute();
	this->responsecode = handlecode.getResponseCode();
	return (*this);
}

unsigned int	Webserv::HandleCode::getCode() const {
	return (this->code);
}

Webserv::Route	Webserv::HandleCode::getRoute() const {
	return (this->route);
}

unsigned int	Webserv::HandleCode::getResponseCode() const {
	return (this->responsecode);
}
