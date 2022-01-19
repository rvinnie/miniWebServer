#include "Response.hpp"

Response::Response() : _code(200), _status(HEADER), _autoindex(false) {}

Response::~Response() {}

Response::mimeType	Response::_mimeType = initMimeType();

void	Response::form(Request& request) {
	std::string body;
	if (_code == 200) {
		try {
			body = methodHandler(&request);
		} catch (HandlerException &e) {
			std::string err = e.what();
			_code = std::atoi(err.c_str());
		}
	}
	_header << "HTTP/1.1 " << _code << " " << _statusText() << "\r\n"
		<< "Server: NONAME\r\n"
		<< "Date: " << _headerTime() << "\r\n";
	std::string cookie = request.getHeader("cookie");
	if (!cookie.size())
		cookie = "myCookie=" + _bakeCookie();
	_header	<< "Set-Cookie: " << cookie << "\r\n";
	if (_code >= 400) {
		body = request.getServer()->getErrorPage(_code);
		_header << "Content-Type: text/html\r\n";
	}
	else if (_code >= 300) {
		_header << "Location: " << _location << "\r\n\r\n";
		return;
	}
	else if (body.size()) {
		std::size_t pos = body.find("\r\n\r\n");
		if (pos != std::string::npos)
			body.erase(0, pos + 4);
		std::size_t dot = _url.find_last_of(".");
		std::string type;
		if (_autoindex)
			type = "text/html";
		else if (dot != std::string::npos)
			type = _mimeType[_url.substr(dot + 1, 4)];
		if (!type.empty()) {
			_autoindex = false;
			_header << "Content-Type: " << type << "\r\n";
		}
	}
	else {
		_header << "Content-Length: 0\r\n";
		if (_code != 204)
			_header << "Content-Location: " << _url << "\r\n\r\n";
		else
			_header << "\r\n";
		return;
	}
	_header << "Content-Length: " << body.size() << "\r\n\r\n";
	_body << body;
}

bool  Response::isComplete(void) const {
	return _status == DONE;
}

int Response::getChunk(char *buff) {
	int size = 0;
	if (_status == HEADER) {
		_header.read(buff, BUFF_SIZE - 1);
		size = _header.gcount();
		if (_header.eof()) {
			if(_body)
				_status = BODY;
			else
				_status = DONE;
		}
	}
  	else if (_status == BODY) {
		_body.read(buff, BUFF_SIZE - 1);
		size = _body.gcount();
		if (_body.eof()) {
			_status = DONE;
		}
	}
	return size;
}

void	Response::setCode(int code) {
	_code = code;
}

int	Response::getCode( void ) {
	return _code;
}

void	Response::setUrl(std::string path) {
	_url = path;
}

void	Response::setLocation(std::string location) {
	_location = location;
}


std::string	Response::_statusText() {
	switch(_code) {
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 204:
			return "No Content";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 307:
			return "Temporary Redirect";
		case 400:
			return "Bad Request";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 408:
			return "Request Timeout";
		case 411:
			return "Length Required";
		case 413:
			return "Request Entity Too Large";
		case 500:
		   	return "Internal Server Error";
		case 501:
		   	return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 505:
			return "HTTP Version Not Supported";
		default:
			break;
	}
	return "I'm a teapot"; //418
}

std::string	Response::_headerTime() {
	char buf[BUFF_SIZE];
	std::time_t t = std::time(nullptr);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", std::localtime(&t));
	std::string ret(buf);
	return ret;
}

std::string	Response::_bakeCookie() {
	std::srand(time(NULL));
  	int nbr = std::rand() % 3;
	if (!nbr)
		return "gingersnap";
	else if (nbr == 1)
		return "snickerdoodle";
	return "macaron";
}

std::stringstream *Response::generateAutoindex(std::string rootPath, std::string directory) {
	int pos = directory.find_last_of("/");
	std::string short_directory = directory.substr(pos, directory.size());
	if (strcmp(directory.c_str(), "/"))
		directory += "/";
	std::string fullPath = rootPath + directory;
	std::stringstream page;
	DIR * dir = opendir(fullPath.c_str());
	struct dirent *entity;
	
	if (!dir) {
		this->setCode(404);
		return (new std::stringstream(page.str()));
	}

	page << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head>\n"
		<< "<link href=\"autoindex.css\" rel=\"stylesheet\">"
		<< "<title>" << short_directory + "/" << "</title>\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "<h1>" << short_directory + "/" << "</h1>"
		<< "<ul class=\"list6b\">\n";
	while ((entity = readdir(dir))) {
		if (!strcmp(entity->d_name, ".") || (entity->d_name[0] == '.' && !strcmp(directory.c_str(), "/")))
			continue ;
		page << "<li><a href=\"" << directory << entity->d_name << "\">"
			<< entity->d_name << "</a></li>\n";
	}
	page << "</ul>\n"
		<< "</body>\n"
		<< "</html>";
	closedir(dir);
	_autoindex = true;
	return (new std::stringstream(page.str()));
}
