#include "ConfigParser.hpp"

// CONSTRUCTOR AND DESTRUCTOR ==================================================

ConfigParser::ConfigParser(std::string path) {
	this->_data = new t_config_data();
	this->_servers = new std::list<ServerData *>();
	this->_allServerNames = new std::vector<std::string>();
	this->_countServers = 0;
	this->_path = path;
}

ConfigParser::~ConfigParser(void) {
	delete this->_data;
	std::list<ServerData *>::iterator it;
	for (it = this->_servers->begin(); it != this->_servers->end(); it++)
		delete *it;
	delete this->_servers;
	delete this->_allServerNames;
}


// MAIN FUNCTIONS ==============================================================

void	ConfigParser::launchConfig(void) {
	// launches main parser
	int start = 0;
	std::list<ServerData *>::iterator it;

	fillData();
	checkValid();
	for (int x = 0; x < this->_countServers; x++)
		createNextServer(&start);
	for (it = this->_servers->begin(); it != this->_servers->end(); it++)
		(*it)->launchServerData();
	checkDuplicateServers();
}

void	ConfigParser::fillData(void) {
	// fills in information from config file in _data
	std::ifstream in(this->_path);
	std::vector<std::string> tmpVector;
	std::string tmpStr;

	if (!in.is_open())
		throw WrongConfigException("unable to find config file");
	while (std::getline(in, tmpStr)) {
		tmpVector = ft_split(tmpStr);
		if (tmpVector.size()) {
			this->_data->push_back(tmpVector);
		}
	}
}

void	ConfigParser::checkValid(void) {
	// checks config file
	std::vector<std::string> tmpVect;
	std::string lineFirstStr;
	std::string lineLastStr;
	int countLeft = 0;
	int countRight = 0;

	for (unsigned long i = 0; i < _data->size(); i++) {
		tmpVect = (*_data)[i];
		lineFirstStr = tmpVect[0];
		lineLastStr = tmpVect[tmpVect.size() - 1];
		if (lineFirstStr == "server") {
			this->_countServers++;
		}
		if (lineLastStr == "{")
			countLeft++;
		else if (lineLastStr == "}")
			countRight++;
		else if (lineLastStr.c_str()[lineLastStr.size() - 1] != ';')
			throw WrongConfigException("simple directive must end with semicolon");
		if (!(isSpecialWord(lineFirstStr)))
			throw WrongConfigException("wrong name of parameter");
	}
	if (countLeft != countRight)
		throw WrongConfigException("missed '{' or '}'");
	if (this->_countServers == 0)
		throw WrongConfigException("unable to find server");
}

void	ConfigParser::createNextServer(int *start) {
	// sets next server into _servers
	std::vector<std::string> line;
	int	startOfServer;
	int	endOfServer;
	int countLeft = 0;
	int countRight = 0;

	while ((*_data)[*start][0] != "server")
		(*start)++;
	startOfServer = *start;
	if ((*_data)[*start].size() == 2 && (*_data)[*start][1] == "{")
		countLeft++;
	(*start)++;
	while (countLeft != countRight) {
		line = (*_data)[*start];
		if (line[line.size() - 1] == "{")
			countLeft++;
		else if (line[line.size() - 1] == "}")
			countRight++;
		(*start)++;
	}
	endOfServer = (*start) - 1;
	if (startOfServer >= endOfServer)
		throw WrongConfigException();
	*start = endOfServer;
	this->_servers->push_back(new ServerData(startOfServer, endOfServer, this->_data));
}

void	ConfigParser::checkDuplicateServers(void) {
	// checks for duplicate locations in _locations
	std::list<ServerData *>::iterator	it;
	std::vector<std::string>::iterator	serverNamesIt;
	std::vector<std::string>	serverNames;

	for (it = this->_servers->begin(); it != this->_servers->end(); it++) {
		serverNames = (*it)->getServerNames();
		for (serverNamesIt = serverNames.begin(); serverNamesIt != serverNames.end(); serverNamesIt++) {
		if (std::find(this->_allServerNames->begin(), this->_allServerNames->end(), *serverNamesIt) != this->_allServerNames->end()) {
			delete *it;
			this->_servers->erase(it);
			throw WrongConfigException("duplicate servers");
		}
		else
			this->_allServerNames->push_back(*serverNamesIt);
	}
	}
}


// SECONDARY FUNCTIONS =========================================================

bool	ConfigParser::isSpecialWord(std::string word) {
	std::string	specialWords[] = {
								"server", "location", "host", "port", "cgi",
								"server_names", "error_page", "max_client_body_size",
								"methods", "root", "index", "autoindex", "}", "redirect", "upload",
								};
	int	size = sizeof(specialWords) / sizeof(*specialWords);
	int	i = 0;
	while (i < size) {
		if (word == specialWords[i++])
			return (true);
	}
	return (false);
}

void	ConfigParser::printInfo(void) const {
	std::cout << COLOR_CYAN << "-=-=-= Config =-=-=-\n\n" << COLOR_RESET;
	std::list<ServerData *>::iterator it;
	for (it = this->_servers->begin(); it != this->_servers->end(); it++)
		(*it)->printInfo();
	std::cout << COLOR_CYAN << "-=-=-=-=-=-=-=-=-=-\n" << COLOR_RESET;
}

// GETTERS AND SETTERS =========================================================

std::list<ServerData *>		&ConfigParser::getServers(void) const {
	return (*this->_servers);
}

int							ConfigParser::getCountServers(void) const {
	return (this->_countServers);
}
