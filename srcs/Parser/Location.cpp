#include "Location.hpp"

// CONSTRUCTOR AND DESTRUCTOR ==================================================

Location::Location(int start, int end, t_config_data *data) {
	this->_paths =  new std::vector<std::string>();
	this->_autoindex = false;
	this->_isGet = false;
	this->_isPost = false;
	this->_isPut = false;
	this->_isDelete = false;
	this->_start = start;
	this->_end = end;
	this->_data = data;
	this->_maxBodySize = INT_MAX;
}

Location::~Location(void) {
	delete this->_paths;
}

// MAIN FUNCTIONS ==============================================================

void	Location::launchLocation(void) {
	// launches location parser
	fillPaths();
	fillInfo();
}

void	Location::fillPaths(void) {
	// fills _paths of current location
	std::vector<std::string>	line;
	size_t						i;

	line = this->_data->at(this->_start);
	if (line.size() < 2)
		throw WrongConfigException("wrong location");
	if (line[1] == "*")
		this->_type = 2;
	else
		this->_type = 1;
	i = 1;
	while (i < line.size()) {
		if (line[i] != "*" && line[i] != "{")
			this->_paths->push_back(line[i]);
		i++;
	}
	if (!this->_paths->size())
		throw WrongConfigException("wrong location");
}

void	Location::fillInfo(void) {
	// fills in the information in the instance
	std::vector<std::string> currentLine;

	for (int x = this->_start; x <= this->_end; x++) {
		currentLine = (*_data)[x];
		if (currentLine[0] == "root" && currentLine.size() == 2)
			this->_root = removeSemi(currentLine[1]);
		else if (currentLine[0] == "index" && currentLine.size() == 2)
			this->_index = removeSemi(currentLine[1]);
		else if (currentLine[0] == "cgi" && currentLine.size() == 2)
			this->_cgi = removeSemi(currentLine[1]);
		else if (currentLine[0] == "upload" && currentLine.size() == 2)
			this->_upload = removeSemi(currentLine[1]);
		else if (currentLine[0] == "max_client_body_size" && currentLine.size() == 2) {
			this->_maxBodySize = convertToInt(currentLine[1]);
		}
		else if (currentLine[0] == "autoindex" && currentLine.size() == 2) {
			if (removeSemi(currentLine[1]) == "1")
				this->_autoindex = true;
			else if (removeSemi(currentLine[1]) == "0")
				this->_autoindex = false;
			else
				throw WrongConfigException("wrong autoindex");
		}
		else if (currentLine[0] == "redirect" && currentLine.size() == 2) {
			this->_redirect = removeSemi(currentLine[1]);
		}
		else if (currentLine[0] == "methods") {
			for (size_t x = 1; x < currentLine.size(); x++) {
				if (removeSemi(currentLine[x]) == "GET")
					this->_isGet = true;
				else if (removeSemi(currentLine[x]) == "POST")
					this->_isPost = true;
				else if (removeSemi(currentLine[x]) == "DELETE")
					this->_isDelete = true;
				else if (removeSemi(currentLine[x]) == "PUT")
					this->_isPut = true;
				else
					throw WrongConfigException("not supported method");
			}
		}
	}
	if (!_isPost && !_cgi.empty())
		throw WrongConfigException("impossible to execute cgi");
	if (_type == 2 && _root.empty() && _cgi.empty())
		throw WrongConfigException("required parameter root for regular expression");
}

// SECONDARY FUNCTIONS =========================================================

void	Location::printInfo(void) const {
	std::cout
		<< COLOR_GREEN << "Locations:" << COLOR_RESET << "[";
	for (size_t i = 0; i < this->_paths->size(); i++)
		std::cout << this->_paths->at(i) << " ";
	
	std::cout << "]" << std::endl
		<< this->_type << " "
		<< this->_root << " "
		<< this->_index << " a"
		<< this->_autoindex << " "
		<< this->_isGet << " "
		<< this->_isPost << " "
		<< this->_isDelete << " r"
		<< this->_redirect << " \n";
	std::cout << "upload: " << this->_upload << std::endl;
	std::cout << "redirect: " << this->_redirect << std::endl;
	std::cout << "max-size: " << this->_maxBodySize << std::endl;
}

// GETTERS AND SETTERS =========================================================

bool	Location::getIsGet(void) const {
	return (this->_isGet);
}

bool	Location::getIsPut(void) const {
	return (this->_isPut);
}

bool	Location::getIsPost(void) const {
	return (this->_isPost);
}

bool	Location::getIsDelete(void) const {
	return (this->_isDelete);
}

bool	Location::getAutoindex(void) const {
	return (this->_autoindex);
}

int	Location::getMaxBodySize(void) const {
	return (this->_maxBodySize);
}

int	Location::getType(void) const {
	return (this->_type);
}

std::string	Location::getRoot(void) const {
	return (this->_root);
}

std::string	Location::getIndex(void) const {
	return (this->_index);
}

std::string	Location::getCgi(void) const {
	return (this->_cgi);
}

std::string	Location::getUpload(void) const {
	return (this->_upload);
}

std::string	Location::getRedirect(void) const {
	return (this->_redirect);
}

std::string	Location::getAutoindexDir(void) const {
	return (this->_autoindexDir);
}

std::vector<std::string>	Location::getPaths(void) const {
	return (*this->_paths);
}

void	Location::setPath(std::string path) {
	this->_paths->push_back(path);
}

void	Location::setAutoindexDir(std::string dir) {
	this->_autoindexDir = dir;
}

void	Location::setPaths(std::vector<std::string> paths) {
	this->_paths->insert(this->_paths->end(), paths.begin(), paths.end());
}
