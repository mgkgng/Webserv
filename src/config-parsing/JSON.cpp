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

int JSON::isbooleanfalse(std::ifstream & file) {
	char c = file.peek();
	if (tolower(c) == 'a') {
		file.get(c);
		c = file.peek();
		if (tolower(c) == 'l') {
			file.get(c);
			c = file.peek();
			if (tolower(c) == 's') {
				file.get(c);
				c = file.peek();
				if (tolower(c) == 'e') {
					file.get(c);
					c = file.peek();
					if (isspace(c) || c == ',' || c == '}') {
						return (1);
					}
				}
			}
		}
	}
	file.close();
	throw InvalidJSONWrongValueType();
}

int JSON::isbooleantrue(std::ifstream & file) {
	char c = file.peek();
	if (c == 'r') {
		file.get(c);
		c = file.peek();
		if (c == 'u') {
			file.get(c);
			c = file.peek();
			if (c == 'e') {
				file.get(c);
				c = file.peek();
				if (isspace(c) || c == ',' || c == '}') {
					return (1);
				}
			}
		}
	}
	file.close();
	throw InvalidJSONWrongValueType();
}

int JSON::isnull(std::ifstream & file) {
	char c = file.peek();
	if (c == 'u') {
		file.get(c);
		c = file.peek();
		if (c == 'l') {
			file.get(c);
			c = file.peek();
			if (c == 'l') {
				file.get(c);
				c = file.peek();
				if (isspace(c) || c == ',' || c == '}') {
					return (1);
				}
			}
		}
	}
	file.close();
	throw InvalidJSONWrongValueType();
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
				switch (c) {
					case '"':
						this->strings.insert(std::pair<std::string, std::string>(str, parsestring(file)));
						file.get(c);
						break;
					case '{':
						// this->objects.insert(std::pair<std::string, JSON>(str, JSON(file)));
						break;
					case 't':
						if (isbooleantrue(file) == 1) {
							this->booleans.insert(std::pair<std::string, bool>(str, true));
						}
						break;
					case 'f':
						if (isbooleanfalse(file) == 1) {
							this->booleans.insert(std::pair<std::string, bool>(str, false));
						}
						break;
					case 'n':
						if (isnull(file) == 1) {
							this->nulls.push_back(str);
						}
						break;
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
				if (c == ',') { notdone = true; } else {notdone = false;}
			}
			if (!file.eof()) {
				file.close();
				throw InvalidJSONMulitipleRootsFound();
			}
		} else {
			throw InvalidJSONWrongRootType();
		}
	}
	file.close();
}

JSON::~JSON() { }

const JSON::string_box JSON::getStrings() const {
	return this->strings;
}

const JSON::boolean_box JSON::getBooleans() const {
	return this->booleans;
}
