#include "ServerData.hpp"

// CONSTRUCTOR AND DESTRUCTOR ==================================================

ServerData::ServerData(int start, int end, t_config_data *data) {
	this->_locations = new std::list<Location *>();
	this->_allPaths = new std::vector<std::string>();
	this->_serverNames = new std::vector<std::string>();
	this->_serverNames->push_back(SERVER_NAME);
	this->_countLocations = 0;
	this->_port = -1;
	this->_host = "127.0.0.1";
	this->_maxClientBodySize = -1;
	this->_errors[400] = PATH_TO_400;
	this->_errors[403] = PATH_TO_403;
	this->_errors[404] = PATH_TO_404;
	this->_errors[405] = PATH_TO_405;
	this->_errors[408] = PATH_TO_408;
	this->_errors[411] = PATH_TO_405;
	this->_errors[413] = PATH_TO_413;
	this->_errors[418] = PATH_TO_418;
	this->_errors[500] = PATH_TO_500;
	this->_errors[501] = PATH_TO_501;
	this->_errors[502] = PATH_TO_502;
	this->_errors[505] = PATH_TO_505;
	this->_start = start;
	this->_end = end;
	this->_data = data;
}

ServerData::~ServerData(void) {
	std::list<Location *>::iterator	it;
	for (it = this->_locations->begin(); it != this->_locations->end(); it++)
		delete *it;
	delete this->_locations;
	delete this->_allPaths;
	delete this->_serverNames;
}

// MAIN FUNCTIONS ==============================================================

std::string	ServerData::getErrorPage(int code) {
	// return error page by code
	std::ostringstream errorPage;
	std::string filename = this->_errors[code];
	std::ifstream ifs("www/webserv.ru/errors/" + filename);
	if (!ifs.is_open())
		std::cerr << "Error page is not found" << std::endl;
	errorPage << ifs.rdbuf();
	ifs.close();
	return (errorPage.str());
}

bool	compareTypes(const Location *l, const Location *r) {
	// function for sort locations list
	return (r->getType() < l->getType());
}

void	ServerData::launchServerData(void) {
	// launches server parser
	int start = this->_start;
	std::list<Location *>::iterator it;

	fillInfo();
	for (int x = 0; x < this->_countLocations; x++)
		createNextLocation(&start);
	for (it = this->_locations->begin(); it != this->_locations->end(); it++)
		(*it)->launchLocation();
	checkInfo();
	(this->_locations)->sort(compareTypes);
}

void	ServerData::addError(std::string code, std::string description) {
	std::ifstream ifs("www/webserv.ru/errors/" + description);
	if (!ifs.is_open())
		throw WrongConfigException("wrong error page");
	this->_errors[std::atoi(code.c_str())] = description;
	ifs.close();
}

void	ServerData::fillInfo(void) {
	// fills in the information in the instance
	std::vector<std::string> currentLine;
	for (int x = this->_start; x <= this->_end; x++) {
		currentLine = (*_data)[x];
		if (currentLine[0] == "host" && currentLine.size() == 2)
			this->_host = removeSemi(currentLine[1]);
		else if (currentLine[0] == "port" && currentLine.size() == 2) {
			if (this->_port > 0)
				throw WrongConfigException("multiple ports per server");
			this->_port = convertToInt(currentLine[1]);
		}
		else if (currentLine[0] == "max_client_body_size" && currentLine.size() == 2 && this->_maxClientBodySize < 0) {
			this->_maxClientBodySize = convertToInt(currentLine[1]);
		}
		else if (currentLine[0] == "error_page" && currentLine[1].size() && currentLine.size() == 3)
			addError(currentLine[1], removeSemi(currentLine[2]));
		else if (currentLine[0] == "location")
			this->_countLocations++;
		else if (currentLine[0] == "server_names") {
			this->_serverNames->clear();
			for (size_t i = 1; i < currentLine.size(); i++)
				this->_serverNames->push_back(removeSemi(currentLine.at(i)));
		}
	}
}

void	ServerData::createNextLocation(int *start) {
	// create next location in _locations
	std::vector<std::string> line;
	int	startOfLocation;
	int	endOfLocation;

	while ((*this->_data)[*start][0] != "location")
		(*start)++;
	startOfLocation = *start;
	line = (*this->_data)[*start];
	while (line[line.size() - 1] != "}") {
		(*start)++;
		line = (*this->_data)[*start];
	}
	endOfLocation = *start;
	if (startOfLocation >= endOfLocation)
		throw WrongConfigException();
	*start = endOfLocation;
	this->_locations->push_back(new Location(startOfLocation, endOfLocation, this->_data));
}

void	ServerData::checkInfo(void) {
	// checks data in instance
	std::list<Location *>::iterator	it;

	if (this->_port == -1 || this->_port == 0 || this->_maxClientBodySize == -1
		|| this->_maxClientBodySize == 0 || !this->_host.size() || _countLocations == 0) {
		throw WrongConfigException("incomplete server parameters");
	}
	for (it = this->_locations->begin(); it != this->_locations->end(); it++) {
		checkDuplicateLocations(it);
	}
}

void	ServerData::checkDuplicateLocations(std::list<Location *>::iterator & it) {
	// checks for duplicate locations in _locations
	std::vector<std::string> vect = (*it)->getPaths();
	std::vector<std::string>::iterator vectIt;

	for (vectIt = vect.begin(); vectIt != vect.end(); vectIt++) {
		if (std::find(this->_allPaths->begin(), this->_allPaths->end(), *vectIt) != this->_allPaths->end()) {
			delete *it;
			this->_locations->erase(it);
			throw WrongConfigException("duplicate locations");
		}
		else
			this->_allPaths->push_back(*vectIt);
	}
}

void	ServerData::setLocationsRedirect(std::vector<std::string> const & what, std::string const & where) {
	// sets redirects in _locations
	std::list<Location *>::iterator it;
	std::vector<std::string> paths;

	for (it = this->_locations->begin(); it != this->_locations->end(); it++) {
		paths = (*it)->getPaths();
		if (paths.end() != std::find(paths.begin(), paths.end(), where)) {
			(*it)->setPaths(what);
		}
	}
}

// SECONDARY FUNCTIONS =========================================================

void	ServerData::printInfo(void) const {
	std::cout
			<< COLOR_YELLOW << "<----- Server ----->\n" << COLOR_RESET
			<< this->_host << std::endl
			<< this->_port << std::endl
			<< this->_maxClientBodySize << std::endl;
	std::vector<std::string>::iterator it2;
	std::cout << COLOR_MAGENTA<< "Server_names: " << COLOR_RESET;
	for (it2 = this->_serverNames->begin(); it2 != this->_serverNames->end(); it2++)
		std::cout << *it2 << " ";
	std::cout << std::endl;
	std::list<Location *>::iterator it;
	for (it = this->_locations->begin(); it != this->_locations->end(); it++)
		(*it)->printInfo();
	std::cout << COLOR_YELLOW << "<------------------>\n" << COLOR_RESET << std::endl;
}

// GETTERS AND SETTERS =========================================================

int	ServerData::getPort(void) const {
	return (this->_port);
}

std::string	ServerData::getHost(void) const {
	return (this->_host);
}

std::vector<std::string>	ServerData::getServerNames(void) const {
	return (*this->_serverNames);
}

int	ServerData::getMaxBodySize(void) const {
	return (this->_maxClientBodySize);
}

std::map<int, std::string>	ServerData::getErrors(void) const {
	return (this->_errors);
}

std::list<Location *>	ServerData::getLocations(void) const {
	return (*this->_locations);
}
