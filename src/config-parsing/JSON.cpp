#include <JSON.hpp>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <algorithm>

JSON::JSON(): strings(), numbers(), booleans(), objects() { }

void JSON::skipwhitespace(std::ifstream & file) {
	if (file.is_open() && file.good()) {
		char c;
		c = file.peek();
		while (!file.eof() && isspace(c)) {
			file.get(c);
			c = file.peek();
		}
	}
}

std::string JSON::parsestring(std::ifstream & file) {
	char c;
	std::string str = "";

	while(file.peek() != '"') {
		file.get(c);
		if (file.eof()) {
			file.close();
			throw InvalidJSONUnclosedQuotations();
		}
		if (c == '\\') {
			file.get(c);
			switch (c) {
				case '"':
					str.push_back('"');
					break;
				case '\\':
					str.push_back('"');
					break;
				case '/':
					str.push_back('"');
					break;
				case 'n':
					str.push_back('"');
					break;
				case 't':
					str.push_back('"');
					break;
				case 'b':
					str.push_back('"');
					break;
				case 'f':
					str.push_back('"');
					break;
				case 'r':
					str.push_back('"');
					break;
			}
		} else if (c == '\n') {
			file.close();
			throw InvalidJSON();
		} else {
			str.push_back(c);
		}
	}
	return str;
}

JSON::JSON(std::string path) {
	std::ifstream	file;
	char			c;

	if (access(path.c_str(), F_OK | R_OK) == -1) {
		throw InvalidPath();
	}
	file.open(path);
	skipwhitespace(file);
	if (!file.eof()) {
		file.get(c);
		if (c == '{') {
			bool notdone = true;
			while (!file.eof()) {
				skipwhitespace(file);
				file.get(c);
				if (c == '}') {
					if (notdone == true) {
						file.close();
						throw InvalidJSONWrongKeyType();
					}
					break ;
				} else if (c != '"') {
					file.close();
					throw InvalidJSONWrongKeyType();
				} else if (notdone == false) {
					file.close();
					throw InvalidJSONWrongObjectEnd();
				}
				std::string str = parsestring(file);
				if (std::find(this->keys.begin(), this->keys.end(), str) != keys.end()) {
					file.close();
					throw InvalidJSONDuplicateKeyNames();
				}
				keys.push_back(str);
				file.get(c);
				skipwhitespace(file);
				file.get(c);
				if (c != ':') {
					file.close();
					throw InvalidJSONWrongSepteratorCharacter();
				}
				skipwhitespace(file);
				file.get(c);
				switch (tolower(c)) {
					case '"':
						this->strings.insert(std::pair<std::string, std::string>(str, parsestring(file)));
						file.get(c);
						break;
					case '{':
						break;
					case 't':
						break;
					case 'f':
						break;
					// case 'n':
					// 	break;
					default:
						if (c == '-' || isdigit(c)) {

						} else {
							file.close();
							throw InvalidJSONWrongValueType();
						}
						break;
				}
				skipwhitespace(file);
				file.get(c);
				if (c == ',') { notdone = true; file.get(c); } else {notdone = false;}
			}
			if (!file.eof()) {
				file.close();
				throw InvalidJSONMulitipleRootsFound();
			}
		} else {
			throw InvalidJSONWrongRootType();
		}
	}
}

JSON::~JSON() { }

const JSON::string_box JSON::getStrings() const {
	return this->strings;
}
