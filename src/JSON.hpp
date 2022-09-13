#pragma once
#include "utility.hpp"

class JSON {
	public:
		typedef std::map<std::string, std::string>					string_box;
		typedef std::map<std::string, long double>					number_box;
		typedef std::map<std::string, bool>							boolean_box;
		typedef std::vector<std::pair<std::string, JSON> >			object_box;

		// shorter versions of names for data storage
		JSON(): strings(), numbers(), booleans(), objects() { }

		std::string find_extension_parser(std::string path) {
			std::string temp = path;
			while (temp.find(".") != std::string::npos) {
				temp.erase(0, temp.find(".") + 1);
			}
			if (path.length() == temp.length()) {
				return "";
			}
			if (temp.find('/') != std::string::npos) {
				return "";
			}
			return temp;
		}

		JSON(std::string path) {
			std::ifstream	file;
			char			c;

			if (access(path.c_str(), F_OK | R_OK) == -1) {
				throw InvalidPath();
			} else if (find_extension_parser(path) != "json") {
				throw InvalidExtension();
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
								this->objects.push_back(std::pair<std::string, JSON>(str, JSON(file)));
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
									this->numbers.insert(std::pair<std::string, long double>(str, parsenumber(file, c)));
								} else {
									file.close();
									throw InvalidJSONWrongValueType();
								}
								break;
						}
						keys.push_back(str);
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

		JSON(const JSON & json) {
			this->strings = json.getStrings();
			this->objects = json.getObjects();
			this->booleans = json.getBooleans();
			this->keys = json.getKeys();
			this->numbers = json.getNumbers();
			this->nulls = json.getNulls();
		}

		JSON & operator=(const JSON & json) {
			this->strings = json.getStrings();
			this->objects = json.getObjects();
			this->booleans = json.getBooleans();
			this->keys = json.getKeys();
			this->numbers = json.getNumbers();
			this->nulls = json.getNulls();
			return (*this);
		}

		~JSON() { }

		const string_box getStrings() const {
			return this->strings;
		}

		const boolean_box getBooleans() const {
			return this->booleans;
		}

		const number_box getNumbers() const {
			return this->numbers;
		}

		const object_box getObjects() const {
			return this->objects;
		}

		const std::vector<std::string> getNulls() const {
			return this->nulls;
		}

		const std::vector<std::string> getKeys() const {
			return this->keys;
		}

	protected:
		// Recursive Constructor (Should never be called outside of string constructor)
		JSON(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			skipwhitespace(file);
			char c = file.peek();
			bool notdone = true;
			if (c == '}') {
				notdone = false;
				file.get(c);
			}
			while (c != '}' && !file.eof()) {
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
						this->objects.push_back(std::pair<std::string, JSON>(str, JSON(file)));
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
							this->numbers.insert(std::pair<std::string, long double>(str, parsenumber(file, c)));
						} else {
							file.close();
							throw InvalidJSONWrongValueType();
						}
					}
				keys.push_back(str);
				skipwhitespace(file);
				file.get(c);
				if (c == ',') { notdone = true; } else {notdone = false;}
			}
			if (file.eof()) {
				file.close();
				throw InvalidJSONWrongObjectEnd();
			}
		}
		// Data
		string_box					strings;
		number_box					numbers;
		boolean_box					booleans;
		object_box					objects;
		std::vector<std::string>	nulls;
		
		// All keys
		std::vector<std::string>	keys;

		// Functions used to parse file into data
		void skipwhitespace(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			char c;
			c = file.peek();
			while (!file.eof() && isspace(c)) {
				file.get(c);
				c = file.peek();
			}
		}

		std::string parsestring(std::ifstream & file) {
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

		long double parsenumber(std::ifstream & file, char c) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			std::string temp;
			bool hasdot = false;
			temp.push_back(c);
			if (c == '0' && isdigit(file.peek())) {
				file.close();
				throw ;
			}
			c = file.peek();
			while ((isdigit(c) || c == '.') && !file.eof()) {
				temp.push_back(c);
				if (c == '.' && hasdot == false) {
					hasdot = true;
					file.get(c);
					c = file.peek();
					if (isdigit(c) && file.eof()) {
						file.close();
						throw ;
					}
				} else if (c == '.' && hasdot) {
					file.close();
					throw ;
				} else {
					file.get(c);
					c = file.peek();
				}
			}
			if (tolower(c) == 'e') {
				temp.push_back(c);
				file.get(c);
				c = file.peek();
				if (c == '+' || c == '-') {
					temp.push_back(c);
					file.get(c);
					c = file.peek();
					while (isdigit(c) && !file.eof()) {
						temp.push_back(c);
						file.get(c);
						c = file.peek();
					}
				} else {
					file.close();
					throw ;
				}
			}
			if (!isspace(c) && file.eof()) {
				file.close();
				throw ;
			}
			return (strtold(temp.c_str(), NULL));
		}

		int isbooleanfalse(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
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

		int isbooleantrue(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
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

		int isnull(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
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
		// Errors related to IO
		struct InvalidPath: public std::exception { const char * what () const throw () { return "No file was found from the given path"; } };
		struct ErrorReadingFile: public std::exception { const char * what () const throw () { return "A fatal error was encountered while reading the file"; } };
		struct InvalidExtension: public std::exception {const char * what () const throw () { return "Invalid extension, only .json files are allowed!"; } };

		// Errors related to Incorectly Formated JSON
		struct InvalidJSON: public std::exception { const char * what () const throw () { return "An error with the JSON was found"; } };
		struct InvalidJSONWrongRootType: public std::exception { const char * what () const throw () { return "Expected \"{\" but got something else"; } };
		struct InvalidJSONWrongObjectEnd: public std::exception { const char * what () const throw () { return "Expected \"}\" but got something else"; } };
		struct InvalidJSONWrongKeyType: public std::exception { const char * what () const throw () { return "Expected STRING but got something else"; } };
		struct InvalidJSONWrongValueType: public std::exception { const char * what () const throw () { return "Expected STRING, OBJECT, TRUE, FALSE, NULL or NUMBER but got something else"; } };
		struct InvalidJSONWrongSepteratorCharacter: public std::exception { const char * what () const throw () { return "Expected \":\" but got something else"; } };
		struct InvalidJSONMulitipleRootsFound: public std::exception { const char * what () const throw () { return "Multiple root founds, store multiple VALUEs in an OBJECT"; } };
		struct InvalidJSONUnclosedQuotations: public std::exception { const char * what () const throw () { return "An unclosed quote was discovered"; } };
		struct InvalidJSONDuplicateKeyNames: public std::exception { const char * what () const throw () { return "Two Different KEYs share the same name!"; } };
};