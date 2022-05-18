#ifndef JSON_HPP
#define JSON_HPP

#include <string>
#include <map>
#include <vector>

class JSON {
	public:
		// shorter versions of names for data storage
		typedef std::map<std::string, std::string>					string_box;
		typedef std::map<std::string, long double>					number_box;
		typedef std::map<std::string, bool>							boolean_box;
		typedef std::map<std::string, JSON>							object_box;

		JSON();
		JSON(std::string path);
		JSON(const JSON & json);
		~JSON();
		JSON & operator=(const JSON & json);
		const string_box getStrings() const;
		const number_box getNumbers() const;
		const boolean_box getBooleans() const;
		const object_box getObjects() const;
		const std::vector<std::string> getNulls() const;
		const std::vector<std::string> getKeys() const;

	protected:
		// Recursive Constructor (Should never be called outside of same constructor)
		JSON(std::ifstream & file);

		// Data
		string_box					strings;
		number_box					numbers;
		boolean_box					booleans;
		object_box					objects;
		std::vector<std::string>	nulls;
		
		// All keys
		std::vector<std::string>	keys;

		// Functions used to parse file into data
		void		skipwhitespace(std::ifstream & file);
		std::string	parsestring(std::ifstream & file);
		long double	parsenumber(std::ifstream & file, char c);
		int			isbooleantrue(std::ifstream & file);
		int			isbooleanfalse(std::ifstream & file);
		int			isnull(std::ifstream & file);

		// Errors related to IO
		struct InvalidPath: public std::exception { const char * what () const throw () { return "No file was found from the given path"; } };
		struct ErrorReadingFile: public std::exception { const char * what () const throw () { return "A fatal error was encountered while reading the file"; } };

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

#endif