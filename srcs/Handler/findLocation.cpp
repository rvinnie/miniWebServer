#include "handler.hpp"

static int	checkRegularExp(std::vector<std::string> & curPaths, std::string urn) {
	std::vector<std::string>::iterator it;

	for (it = curPaths.begin(); it != curPaths.end(); it++) {
		if (urn.size() >= ((*it).size())
			&& urn.compare(urn.size() - (*it).size(), (*it).length(), *it) == 0
			&& urn.c_str()[urn.size() - (*it).size() - 1] == '.')
			return (1);
	}
	return (0);
}

static void	rebuildUrn(std::string *urn) {
	int pos = urn->find_last_of("/");
	*urn = urn->substr(0, pos);
}

static int	checkPrefixString(std::list<Location *>::iterator *location, std::vector<std::string> & curPaths, std::string urn) {
	std::vector<std::string>::iterator	it;

	while (urn.find_last_of("/") != std::string::npos) {
		for (it = curPaths.begin(); it != curPaths.end(); it++) {
			if (*it == urn) {
				(**location)->setAutoindexDir(urn);
				return ((*it).size());
			}
		}
		rebuildUrn(&urn);
		
	}
	(**location)->setAutoindexDir(urn);
	for (it = curPaths.begin(); it != curPaths.end(); it++) {
		if (*it == urn || *it == "/")
			return ((*it).size());
	}
	
	return (0);
}

Location *findLocation(std::list<Location *> locations, std::string urn, Request *request) {
	std::list<Location *>::iterator	it;
	std::vector<std::string>		curPaths;
	Location						*bestChoice = NULL;
	int								maxLen = 0;
	int								currentLen;

	for (it = locations.begin(); it != locations.end(); it++) {
		curPaths = (*it)->getPaths();
		if ((*it)->getType() == 2 && request->getMethod() != "PUT" && request->getMethod() != "DELETE") {
			if (checkRegularExp(curPaths, urn)) {
				(*it)->setAutoindexDir(urn);
				return (*it);
			}
		}
		else {
			currentLen = checkPrefixString(&it, curPaths, urn);
			if (currentLen > maxLen) {
				maxLen = currentLen;
				bestChoice = *it;
			}
		}
	}
	return (bestChoice);
}