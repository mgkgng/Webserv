#include "mime.hpp"
#include "utility.hpp"

MimeMap::MimeMap() {
	mime["aac"] = "audio/aac";
	mime["abw"] = "application/x-abiword";
	mime["arc"] = "application/x-freearc";
	mime["avif"] = "image/avif";
	mime["avi"] = "video/x-msvideo";
	mime["azw"] = "application/vnd.amazon.ebook";
	mime["bin"] = "application/octet-stream";
	mime["bmp"] = "image/bmp";
	mime["bz"] = "application/x-bzip";
	mime["bz2"] = "application/x-bzip2";
	mime["cda"] = "application/x-cdf";
	mime["csh"] = "application/x-csh";
	mime["css"] = "text/css";
	mime["csv"] = "text/csv";
	mime["doc"] = "application/msword";
	mime["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime["eot"] = "application/vnd.ms-fontobject";
	mime["epub"] = "application/epub+zip";
	mime["gz"] = "application/gzip";
	mime["gif"] = "image/gif";
	mime["htm"] = "text/html";
	mime["html"] = "text/html";
	mime["ico"] = "image/vnd.microsoft.icon";
	mime["ics"] = "text/calendar";
	mime["jar"] = "application/java-archive";
	mime["jpeg"] = "image/jpeg";
	mime["jpg"] = "image/jpeg";
	mime["js"] = "text/javascript";
	mime["json"] = "application/json";
	mime["jsonld"] = "application/ld+json";
	mime["mid"] = "audio/midi";
	mime["midi"] = "audio/midi";
	mime["mjs"] = "text/javascript";
	mime["mp3"] = "audio/mpeg";
	mime["mp4"] = "video/mp4";
	mime["mpeg"] = "video/mpeg";
	mime["mpkg"] = "application/vnd.apple.installer+xml";
	mime["odp"] = "application/vnd.oasis.opendocument.presentation";
	mime["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mime["odt"] = "application/vnd.oasis.opendocument.text";
	mime["oga"] = "audio/ogg";
	mime["ogv"] = "video/ogg";
	mime["ogx"] = "application/ogg";
	mime["opus"] = "audio/opus";
	mime["otf"] = "font/otf";
	mime["png"] = "image/png";
	mime["pdf"] = "application/pdf";
	mime["php"] = "application/x-httpd-php";
	mime["ppt"] = "application/vnd.ms-powerpoint";
	mime["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime["rar"] = "application/vnd.rar";
	mime["rtf"] = "application/rtf";
	mime["sh"] = "application/x-sh";
	mime["svg"] = "image/svg+xml";
	mime["swf"] = "application/x-shockwave-flash";
	mime["tar"] = "application/x-tar";
	mime["tiff"] = "image/tiff";
	mime["tif"] = "image/tiff";
	mime["ts"] = "video/mp2t";
	mime["ttf"] = "font/ttf";
	mime["txt"] = "text/plain";
	mime["vsd"] = "application/vnd.visio";
	mime["wav"] = "audio/wav";
	mime["weba"] = "audio/webm";
	mime["webm"] = "video/webm";
	mime["webp"] = "image/webp";
	mime["woff"] = "font/woff";
	mime["woff2"] = "font/woff2";
	mime["xhtml"] = "application/xhtml+xml";
	mime["xls"] = "application/vnd.ms-excel";
	mime["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime["xml"] = "application/xml";
	mime["xul"] = "application/vnd.mozilla.xul+xml";
	mime["zip"] = "application/zip";
	mime["3gp"] = "video/3gpp";
	mime["3g2"] = "video/3gpp2";
	mime["7z"] = "application/x-7z-compressed";
	mime["wasm"] = "application/wasm";
}

MimeMap::~MimeMap() {};

const char	*mime(const std::string &name) {
	string str = name.substr(name.find_last_of('.') + 1);
	std::map<string, const char *>::const_iterator iter = mimeMap.mime.find(str);
	return (iter == mimeMap.mime.end() ? "text/plain" : iter->second);
}