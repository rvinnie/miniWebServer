#include "Request.hpp"

Request::Request(std::vector<ServerData*> serv) {
	this->_serverVector = serv;
	this->_server = serv[0];

	this->_status = FIRST_LINE;
	this->_chunkStatus = SIZE;
	this->_error = 0;
	this->_headers = std::map<std::string, std::string>();
	findRootPath();
}

Request::~Request() {}

std::string Request::getUrn() {
	return (this->_urn);
}

ServerData* Request::getServer() {
	return (this->_server);
}

Status		Request::getStatus() {
	return (this->_status);
}

size_t		Request::getLength() {
	return (this->_length);
}

size_t		Request::getError() {
	return (this->_error);
}

std::string Request::getBuffer() {
	return (this->_buffer);
}

std::string Request::getMethod() {
	return (this->_method);
}

void	Request::findRootPath() {
	std::string path = getenv("PWD");
	this->_rootPath = path;
}

std::string	Request::getRootPath() {
	return (this->_rootPath);
}

std::string Request::getBody(){
	return (this->_body);
}

std::string Request::getPath(){
	return (this->_path);
}

std::string Request::getVersion(){
	return (this->_version);
}

std::map<std::string, std::string> Request::getHeaders(){
	return (this->_headers);
}
		
std::string	Request::getHeader(std::string header) {
	return (this->_headers[header]);
}

std::string	Request::getQuery() {
	return (this->_query);
}

bool	Request::isVersionValid(){
	if (this->_version == "1.1")
		return true;
	return false;
}


int		Request::parse(std::string& buf) {
	_buffer += buf;
	size_t j = 0;

	switch (_status) {
		case FIRST_LINE: {
			_error = parseFirstLine();
		}
		case HEADERS: {
			_error = parseHeaders();
		}
		case BODY: {
			_error = parseBody();
		}
		case COMPLETE: {
			if (_status == COMPLETE)
				return (_error);
		}
		case ERROR: 
			j = _buffer.find("\r\n\r\n");
			if (j != std::string::npos) {
				_buffer.clear();
				_status = COMPLETE;
			}
			else
				return (_error);
			setServer();
			if (_error == 0)
				_error = 400;
			return (_error);
	}
	return (_error);
}

int		Request::parseFirstLine( void ) {
	size_t				i;
	size_t				j;
	std::string 	firstLine;

	while (_buffer.find("\n") == 0  || _buffer.find("\r") == 0)
		_buffer.erase(0, 1);
	j = _buffer.find("\r\n");
	if (j == std::string::npos)
		return (0);
	firstLine = _buffer.substr(0, j);
	i = firstLine.find(' ');
	if (i == std::string::npos) {
		return (400); 
	}
	this->_method.assign(firstLine, 0, i);
	j = firstLine.find_first_not_of(' ', i);
	if (j == std::string::npos)
		return (400); 
	i = firstLine.find(' ', j);
	if (i == std::string::npos)
		return (400); 
	this->_path.assign(firstLine, j, i - j);
	
	j = this->_path.find('?');
	if (j != std::string::npos) {
		this->_query.assign(this->_path, j + 1, this->_path.size() - j - 1);
		this->_path.erase(j, this->_path.size() - j);
	}
	this->_urn = "/" + this->_path;
	j = firstLine.find_first_not_of(' ', i);
	if (j == std::string::npos)
		return (400); 
	i = firstLine.length();
	if ( (j + 5) >= i || firstLine[j] != 'H' || firstLine[j + 1] != 'T' || firstLine[j + 2] != 'T' || firstLine[j + 3] != 'P' || firstLine[j + 4] != '/')
		return (400); 
	this->_version.assign(firstLine, j + 5, i - j - 5);
	if (!this->isVersionValid())
		return (400);
	_buffer.erase(0, firstLine.length() + 2);
	if ( getMethod() == "GET" || getMethod() == "PUT" || getMethod() == "DELETE" || getMethod() == "POST") {
		this->_status = HEADERS;
		return (0);
	}
	else {
		this->_status = ERROR;
		this->_error = 405;
		return (405);
	}
}

int		Request::parseHeaders( void ) {
	std::string		key;
	std::string		value;
	size_t			i;
	size_t			j;

	while (this->_status == HEADERS) {
		i = _buffer.find("\r\n");
		if (i == std::string::npos)
			break ;
		if (i == 0) {
			_buffer.erase(0, 2);
			checkHeaders();
			break ;
		}
		j = _buffer.find(':');
		if (j == 0)
			return (400);
		key = _buffer.substr(0, j);
		while (key.rfind(' ') == key.length() - 1)
			key.erase(key.length() - 1, 1);
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		value = _buffer.substr(j + 1, i - j - 1);
		while (value.find(' ') == 0)
			value.erase(0, 1);
		while (value.rfind(' ') == value.length() - 1)
			value.erase(value.length() - 1, 1);
		_headers[key] = value;
		_buffer.erase(0, i + 2);
	}
	return (_error);
}

void	Request::setServer( void ) {
	std::string serverName = this->getHeader("host");
	for (size_t i = 0; i < _serverVector.size(); ++i) {
		std::vector<std::string> serverNames = _serverVector[i]->getServerNames();
		for (size_t j = 0; j < serverNames.size(); j++) {
			if (serverNames[j] == serverName) {
				_server = _serverVector[i];
				return ;
			}
		}
	}
	_server = _serverVector[0];
}

void	Request::checkHeaders( void ) {
	int len;
	if (_headers.find("transfer-encoding") != _headers.end() && _headers.find("transfer-encoding")->second == "chunked") {
		_status = BODY;
		setServer();
		parseBody();
	}
	else if (_headers.find("content-length") != _headers.end()) {
		len = std::stoi(_headers.find("content-length")->second);
		if (len < 0 ) {
			_status = ERROR;
			setServer();
		}
		else {
			_length = len;
			if (_headers.find("transfer-encoding") == _headers.end() || _headers.find("transfer-encoding")->second != "chunked") {
				_status = BODY;
				setServer();
				parseBody();
			}
		}
	} else {
		_status = COMPLETE;
	}
}

int		Request::parseBody( void ) {
	if (this->_status == BODY) {
		if (_headers.find("transfer-encoding") != _headers.end() && _headers.find("transfer-encoding")->second == "chunked") 
			parseChunkedBody();
		else if (_buffer.length() >= _length) {
			_body = _buffer.substr(0, _length);
			_buffer.clear();
			_status = COMPLETE;
		}
	}
	return (_error);
}

int		Request::parseChunkedBody( void ) {
	size_t		j;
	std::string size;
	while (_buffer.find("\r\n") != std::string::npos) {
		if (_chunkStatus == SIZE) {
			j = _buffer.find("\r\n");
			if ( j != std::string::npos ) {
				size = _buffer.substr(0, j);
			}
			else {
				break ;
			}
			try {
			_chunkSize = std::stoi(size);
			} catch (const std::exception & e) {
				_status = ERROR;
				_error = 413;
				return 413;
			}
			_buffer.erase(0, j + 2);
			if (_chunkSize == 0) {
				_status = COMPLETE;
				_buffer.clear();
				break ;
			}
			_chunkStatus = CONTENT;	
		}
		if (_chunkStatus == CONTENT) {
			j = _buffer.find("\r\n");
			if (j == std::string::npos)
				return (_error);
			if (_buffer.length() >= _chunkSize + 2) {
				_body.append(_buffer.substr(0, j));
				_buffer.erase(0, j + 2);
				_chunkStatus = SIZE;
			}
		}
	}
	return (_error);
}

void	Request::setComplete() {
	_status = COMPLETE;
}

bool	Request::isComplete() const {
	return _status == COMPLETE;
}
