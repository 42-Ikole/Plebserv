# Notes


## CONFIG PARSING


	Host/Port			=	https://nginx.org/en/docs/http/ngx_http_core_module.html#listen
							listen [server]:[port]
							listen [server]	#defaults to port 80
							listen [port]
							listen localhost:[port]
							Default = listen *:80 | *:8000

	Server_name			=	www.domain.com / domain.com / localhost
							server_name example.com www.example.com ""
							"" resolves back to server chosen by listen
							https://nginx.org/en/docs/http/ngx_http_core_module.html#server_name

	Default error page	=	http://nginx.org/en/docs/http/ngx_http_core_module.html#error_page
							error_page [error]+ [location]
							Default = ----

	max body size 		=	https://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size
							client_max_body_size [size]
							1g 1m 8k 100 0(disable)
							Default = 16k


### LOCATION BLOCK

	Limit HTTP			=	https://nginx.org/en/docs/http/ngx_http_core_module.html#limit_except
							https://www.tutorialspoint.com/http/http_methods.htm
							limit_except [methods]

	Root				=	http://nginx.org/en/docs/http/ngx_http_core_module.html#root
							root [path]
							Default = html

	Autoindex			=	http://nginx.org/en/docs/http/ngx_http_autoindex_module.html
							on off
							Default = off

	Index				=	https://nginx.org/en/docs/http/ngx_http_index_module.html#index
							default: index index.html
							index [file]+
							http://nginx.org/en/docs/http/ngx_http_core_module.html#try_files

___

Location:
- List of HTTP methods 			=
- Directory or file location	= if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is /tmp/www/pouic/toto/pouet
- def file if request is dir	= 
- exec CGI based on file extension:
	- https://en.wikipedia.org/wiki/Common_Gateway_Interface
	- Use Full path as PATH_INFO
	- Unchunck chuncked requests until eof
	- EOF means end of returned data
	- Set Meta variables (see subject)
	- Call cgi with the file as first argument
	- Run in correct relative directory
	- Should work with php-cgi
	- make the route able to accept uploaded files and configure where it should be saved



https://helpx.adobe.com/coldfusion/cfml-reference/reserved-words-and-variables/cgi-environment-cgi-scope-variables/cgi-server-variables.html
https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
https://www.whoishostingthis.com/compare/cgi-access/variables/
https://datatracker.ietf.org/doc/html/rfc3875#section-4.1
https://linux.die.net/man/5/cgi


##	CGI Meta Variables
		AUTH_TYPE			=	BASIC/SSL or null
		CONTENT_LENGTH		=	[-1] / size of message in octets idk idc idgaf imho
		CONTENT_TYPE		=	[content-type (html header)] / [NULL]
		GATEWAY_INTERFACE	=	[CGI/revision]
		PATH_INFO			=	(absolute path naar file toe)
		PATH_TRANSLATED		=	(absolute path naar file toe)
		QUERY_STRING		=	(string in de Uri na de path eg; var1=value1&var2=with%20percent%20encoding)
		REMOTE_ADDR			=	ip of the client
		REMOTE_IDENT		=	remote username retrieved from the server. idk idc idgaf
								ID of user running CGI script
		REMOTE_USER			=	login of the user if authenticated else null
		REQUEST_METHOD		=	[get] / [set]
		REQUEST_URI			=	(uri van request)
		SCRIPT_NAME			=	Returns the part of the URL from the protocol name up to the query string in the
								first line of the HTTP request.
		SERVER_NAME			=	server_name 
		SERVER_PORT			=	port
		SERVER_PROTOCOL		=	HTTP/1.1
		SERVER_SOFTWARE		=	"Plebserv (linux) PHP/7.3.7"

___
## HTTP HEADERS

>	Editors note:	Waarom moeten we sommige headers doen? 
					zit hier enige logica achter en gaan we ze uberhaupt gebruiken?

https://en.m.wikipedia.org/wiki/List_of_HTTP_header_fields

	THE WHAT TO DO LINE	=
							[METHOD] [LOCATION] [HTTPTYPE]
	
	Accept-Language		=	https://en.m.wikipedia.org/wiki/Content_negotiation
							List of acceptable human languages for response. See Content negotiation.
							Accept-Language: [en-US] 
							** Editors note: 404 french requests?? **
	
	Accept-Charsets		=	Character sets that are acceptable.
							Accept-Charsets: utf-8 / ascii / unicode?
	
	Allow				=	The Allow header lists the set of methods supported by a resource.
							Allow: GET, POST, HEAD etc..
	
	Authorization		=	Authentication credentials for HTTP authentication.
							Basic: [auth key]
	
	Content-Language	=	The natural language or languages of the intended audience for the enclosed content
							Content-Language: [da] [nl] [en] etc..
	
	Content-Length		=	The length of the request body in octets (8-bit bytes).
							Content-Length: 295
	
	Content-Type		=	The Media type of the body of the request (used with POST and PUT requests).
							Content-Type: application/x-www-form-urlencoded
							Content-Type: text/html
	
	Date				=	https://datatracker.ietf.org/doc/html/rfc7231#section-7.1.1.1
							The date and time at which the message was originated
							Date: Sun, 06 Nov 1994 08:49:37 GMT
	
	Host				=	The domain name of the server (for virtual hosting), and the TCP port number on which the server is 
							listening. 
							The port number may be omitted if the port is the standard port for the service requested.
							Host: en.wikipedia.org:8080
							Host: en.wikipedia.org
	
	Last-Modified		=	The last modified date for the requested object (in "HTTP-date" format as defined by RFC 7231)
							Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
	
	Location			=	Used in redirection, or when a new resource has been created.
							Location: http://www.w3.org/pub/WWW/People.html
							Location: /pub/WWW/People.html						
	
	Referer				=	This is the address of the previous web page from which a link to the currently requested page was 
							followed. 
							(The word "referrer" has been misspelled in the RFC as well as in most implementations to the point	
							that it has  become standard usage and is considered correct terminology)
							Referer: http://en.wikipedia.org/wiki/Main_Page
	
	Retry-After			=	If an entity is temporarily unavailable, this instructs the client to try again later. Value could be a	
							specified period of time (in seconds) or a HTTP-date.[54]	
							Retry-After: 120
							Retry-After: Fri, 07 Nov 2014 23:59:59 GMT
	
	Server				=	A name for the server
							Server: Apache/2.4.1 (Unix) or Plebserv/x.x.x (Unix)
	
	Transfer-Encoding	=	The form of encoding used to safely transfer the entity to the user. 
							Transfer-Encoding: chunked / compress / deflate / gzip / identity
	
	User-Agent			=	The user agent string of the user agent.
							User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0
	
	WWW-Authenticate	=	Indicates the authentication scheme that should be used to access the requested entity.	
							WWW-Authenticate: Basic
