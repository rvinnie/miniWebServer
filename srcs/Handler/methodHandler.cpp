#include "handler.hpp"
#include "../Response/Response.hpp"

static std::string getLocName(std::string path, std::vector<std::string> locPaths) {
	std::string bestLocName;
	int maxLen = -1;
	for (size_t i = 0; i < locPaths.size(); i++) {
		std::string locName = locPaths[i];
		int curLen = path.rfind(locName);
		if (curLen > maxLen) {
			maxLen = curLen;
			bestLocName = locName;
		}
	}
	return (bestLocName);
}

static std::string	getFilename(std::string path, Location *location) {
	std::string filename;
	std::vector<std::string> locPaths = location->getPaths();
	size_t	f_pos = path.find_last_of(".");
	size_t	s_pos = path.find_last_of("/");

	if (location->getRoot().size()) {
		if (location->getType() == 2) {
			filename = location->getRoot() + path.substr(s_pos, path.size());
		} else {
			filename = path;
			std::string locName = getLocName(path, locPaths);
			filename.replace(0, locName.size(), location->getRoot());
				if (location->getIndex().size() && (f_pos == std::string::npos || s_pos > f_pos))
			filename += ("/" + location->getIndex());
		}
	}
	else
	{
		filename = getLocName(path, locPaths);
		if (location->getIndex().size())
			filename += ("/" + location->getIndex());
		else {
			if (f_pos != std::string::npos && f_pos > s_pos)
				filename += ("/" + location->getIndex());
		}
	}
	return (filename);
}

static std::string methodGetAndPost(Request *request, Location *location, Response * response) {
	std::string page;
	std::string path;
	std::string filename;
	if (location->getCgi().size()) {
		page = cgiHandler(request, location);
	}
	else {
		filename = getFilename(request->getPath(), location); 
		path = request->getRootPath() + filename;
		response->setUrl(location->getRoot() + filename);
		std::ifstream in(path); 
		std::ostringstream sstr;
		if (!in.is_open())
			throw HandlerException("404");
		sstr << in.rdbuf();
		page = sstr.str(); 
		in.close();
	}
	return (page);
}

static void methodDelete(Request *request) {
	std::string path = request->getRootPath() + request->getUrn();
	if (remove(path.c_str()) != 0)
		throw HandlerException("403");
}

static void  methodPut(Location *location, Request *request, Response *response) {
	std::string path = request->getRootPath();
	if (location->getUpload().size()) {
		std::string filename = getFilename(request->getPath(), location);
		path += (location->getUpload() + "/" + filename);
	}
	else
		path += request->getPath();
	std::ifstream ifs(path);
	if (!ifs.is_open())
		response->setCode(201);
	else if (request->getBody().size()) {
		ifs.close();
		response->setCode(200);
	} else {
		ifs.close();
		response->setCode(204);
	}
	response->setUrl(path);
	std::ofstream ofs(path);
	if (!ofs.is_open())
		throw HandlerException("500");
	if (request->getBody().size())
		ofs << request->getBody();
	ofs.close();
}

static void	checkValid(Request *request, Location *location) {
	std::string method = request->getMethod();
	if ((method == "GET" && !location->getIsGet())
		|| (method == "POST" && !location->getIsPost())
		|| (method == "DELETE" && !location->getIsDelete())
		|| (method == "PUT" && !location->getIsPut()))
			throw HandlerException("405");
}

std::string Response::methodHandler(Request *request) {
	std::string page;
	Location *location = findLocation(request->getServer()->getLocations(), request->getPath(), request);
	if (!location) {
		throw HandlerException("404");
	}
	
	int	bodySize = request->getBody().size();
	if (request->getServer()->getMaxBodySize() < bodySize
		|| location->getMaxBodySize() < bodySize) {
		throw HandlerException("413");
	}

	if (location->getRedirect().size()) {
		this->_location = location->getRedirect();
		this->_code = 301;
		return (page);
	}
	if (location->getAutoindex()) {
		std::stringstream *ss = this->generateAutoindex(request->getRootPath(), location->getAutoindexDir());
		page = ss->str();
		delete ss;
		
		return (page);
	}
	checkValid(request, location);
	_url = request->getPath();
	_code = 200;
	if (request->getMethod() == "GET" || request->getMethod() == "POST") {
		page = methodGetAndPost(request, location, this);
	}
	else if (request->getMethod() == "DELETE")
		methodDelete(request);
	else if (request->getMethod() == "PUT")
		methodPut(location, request, this);
	return (page);
}
