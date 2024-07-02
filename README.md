<h1 align="center" style="border-bottom: none; margin-bottom: 0;">
	Webserv: By <a href="https://github.com/mgkgng">min-kang</a>, <a href="https://github.com/SalvatoreSpina">sspina</a> and <a href="https://github.com/Blenderwizard">jrathelo</a>
</h1>

<p align="center">
	<img src="readme_assets/made-with-c++.svg"/>
	<img src="readme_assets/success-125_100.svg"/>
</p>

<p align="center">
	<img src="readme_assets/example_site.png"/>
</p>

## What is this ?

Webserv is a project in the 42 common core. The goal is to code a webserver from scratch, which can host a static website. We used nginx as a reference and the webserv is compatiable with Google Chrome.

With webserv, I learned a lot about the HTTP Protocol, UNIX sockets, and more.

## Features

### The Config

The config file is the instructions on how the server should operate.

The config file closely resembles a JSON object, minus the fact that it doesn't recognize ARRAYS. However it recognizes all other types that JSON objects have, STRINGS, NUMBER, TRUE, FALSE, NULL and other OBJECTS. A typical config file might look like this ...
```json
{
	"root":  ".",
	"server1": {
		"host":  "example.com",
		"port":  2468,
		"server_name":  "example server",
		"client_body_size":  "10M",
		"route static": {
			"path":  "/static",
			"autoindex":  true
		}
	},
	"code 404": {
		"code": 404,
		"redirect": "/404"
	},
	"route 404": {
		"index": "404.html",
		"root": "./var/www"
	}
}
```

In the example above, one server is create for the host ``example.com`` on port ``2468`` with the internal server name being set to ```"example server"```.

The server has one exposed URI route defined as the object ``"route static"`` that exposes the directory of the webserv (defined by ``"root": "."``) on the uri path ``"/static"`` and has an index of all the files.

Servers can inherit information that was describe above them, In the example above, ``"server1"`` inherits ``"root": "."``, ``"code 404"`` and ``"route 404"``. This information can be overridden if redefined within the object. In this case ``"route 404"`` overrides ``"root"`` with ``"./var/www"``.

#### Keywords

| OBJECT | |
|--|--|
|``sever*``|A HTTP server|
|``code*``|A redefinition of how to handle a HTTP code|
|``route*``|Exposed URI Path definition|

| NUMBER | |
|--|--|
|``code``|HTTP Code Number, used by ``code``|
|``port``|Exposed Port, used by ``server``|
|``response_code``|Code that is sent to the user, used by ``code``|

| STRING| |
|--|--|
|``host``|Name of the host url, used by ``server``|
|``path``|Exposed URI Path name, used by ``route``|
|``root``|Where to look for files, used by ``route``|
|``upload_root``|Where to store uploaded files, used by ``route``|
|``index``|First file to display, used by ``route``|
|``method``|List of allowed methods, seperated by spaces, used by ``route``|
|``redirect``|Url to redirect to, used by ``route``|
|``redirect``|Route path to redirect to , used by ``code``|
|``server_name``|Internal name of server, can be NULL, used by ``server``|
|``client_body_size``|Maximum size that a client can send, used by ``route``|
|``allowed_methods``|What HTTP Methods are allowed, each on separated by a space, used by ``route``|
|``redirect``|What path to redirect, used by ``code``|
|``python_cgi_extension``|What file extension to watch for the python cgi, used by ``route``|
|``perl_cgi_extension``|What file extension to watch for the perl cgi, used by ``route``|

|BOOLEAN (TRUE & FALSE)||
|--|--|
|``autoindex``| Show directory contents, used by ``route`` | 
|``uploadable``| If the route can be uploaded to, used by ``route`` | 
