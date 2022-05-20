#include <Webserv.hpp>


// Webserv::HandleCode::HandleCode() {

// }

Webserv::HandleCode::HandleCode(const HandleCode & handlecode) {
	this->code = handlecode.getCode();
	this->route = handlecode.getRoute();
	this->responsecode = handlecode.getResponseCode();
}

Webserv::HandleCode::HandleCode(int code, Webserv::Route route, int responsecode) {
	this->code = code;
	this->route = route;
	this->responsecode = responsecode;
}

Webserv::HandleCode::~HandleCode() {

}

Webserv::HandleCode &  Webserv::HandleCode::operator=(const HandleCode & handlecode) {
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
