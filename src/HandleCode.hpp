#pragma once

#include "utility.hpp"
#include "Route.hpp"

class HandleCode {
	public:
		int code, responsecode;
		Route route;
		
		HandleCode() : responsecode(0) {}
		HandleCode(unsigned int code, unsigned int responsecode, Route & route) : code(code), responsecode(responsecode), route(route) {}
		~HandleCode() {}
};