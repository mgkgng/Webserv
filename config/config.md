# Config Files

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

## Objects
Objects can either be a ``server``, a ``route``, or a ``code``. Servers contain routes and codes.  A``route`` is a exposed ``path`` on the uri that has a ``root``, aka the path towards the directory you want to expose. A ``code`` is a definition of how to handle on of the many HTTP status codes that exist, and point a ``redirect`` towards an existing ``route``. Objects must start with ``server``, ``route``, or ``code`` and ``route`` and ``code`` cannot contain other objects.

## Keywords

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
|``index``|First file to display, used by ``route``|
|``server_name``|Internal name of server, can be NULL, used by ``server``|
|``client_body_size``|Maximum size that a client can send, used by ``route``|
|``allowed_methods``|What HTTP Methods are allowed, each on separated by a space, used by ``route``|
|``redirect``|What path to redirect, used by ``code``|

|BOOLEAN (TRUE & FALSE)||
|--|--|
|``autoindex``| Show directory contents, used by ``route`` | 


 
