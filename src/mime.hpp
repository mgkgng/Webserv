#pragma once
#include "utility.hpp"

struct MimeMap {
	public:
		std::map<std::string, const char *> mime;

		MimeMap();
		~MimeMap();
};

const MimeMap	mimeMap;

const char	*mime(const std::string &name);