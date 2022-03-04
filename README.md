## miniWebServer
🔥 Team project . Implementation of a simple HTTP web server.  
The project implements all stages of the web server. From creating a connection to sending a response to the user. The project also implemented a simplified version of the CGI module. To demonstrate the work of the project, a small website written in html and css was created.

### Usage 
![platform](https://img.shields.io/badge/platform-macOS-brightgreen--brightgreen)   
```
make  
./webserv [configuration file]
```
Example:

```
make  
./webserv configs/webserv.conf
```
If the second parameter is empty, then the default config will be used.  

### Configuration file
The configuration file is made similarly to nginx. It is possible to set ` host, port, max_client_body_size`.  
You can also set your own route to the required file using `location`.  
It is possible to "catch" requests for file extensions (for example: `location * css` block), also set the methods available for each path, the maximum request body, the default response page. It is also possible to enable autolisting (use `autoindex`).  

Config example:
```
server {
	host						0.0.0.0;
	port						80;
	server_names				        webserv.ru;
	max_client_body_size		                100000000;

	location / {
		methods					GET POST;
		root					/www/webserv.ru/pages/;
		index					page.html;
	}

	location /www/webserv.ru/errors {
		methods					GET;
		root					/www/webserv.ru/errors;
	}

	location /cgi-bin/names {
		methods					GET POST;
		max_client_body_size	                100;
		cgi				        /cgi-bin/names/names.py;
	}

	location /www/webserv.ru/images {
		autoindex				1;
	}

	location * jpeg webp gif png ico {
		methods					GET;
		root					/www/webserv.ru/images;
	}

	location * css {
		methods					GET;
		root					/www/webserv.ru/styles;
	}

	location * js {
		methods					GET;
		root					/www/webserv.ru/reveal.js/js;
	}
}
```
### HTTP Methods
The project implements the main HTTP methods.
| method | description |
|---|---|
|GET|requests information from the specified source and does not affect its content|
|POST|used to send data that may affect the content of the resource|
|PUT|loads the content of the request to the URI specified in the request|
|DELETE|deletes the specified resource|

### CGI
In our project, the cgi module is implemented, which processes scripts and dynamically creates HTML pages.
#### *What is CGI?*
CGI is a standard method used to generate dynamic content on web pages. CGI stands for Common Gateway Interface and provides an interface between the HTTP server and programs generating web content. These programs are better known as CGI scripts. They are written in a scripting language.

### Cookies 🍪
The webserver also knows how to create and accept cookies. In this educational version of the project, cookies do not carry important information, but in a more serious implementation they can be replaced with something else.

### Website
To demonstrate the functionality of the web server, a small site was implemented in HTML and CSS.  
![webserv](https://github.com/rvinnie/miniWebServer/blob/main/www/webserv.ru/images/webserv.gif)

### Contributors
⚡ [daniseed](https://github.com/MalinPolina) 
⚡ [obellado](https://github.com/obellado) 
⚡ [rvinnie](https://github.com/rvinnie ) ⚡
