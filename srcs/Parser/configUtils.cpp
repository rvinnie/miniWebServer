#include "parser.hpp"

void	checkParameter(std::string & str) {
	if (str.size() == 1)
		throw WrongConfigException("empty parameter");
}

int		convertToInt(std::string & str) {
	checkParameter(str);
	return (std::atoi(str.c_str()));
}

std::string	removeSemi(std::string & str) {
	checkParameter(str);
	char *tmpStr = const_cast<char *>(str.c_str());
	if (tmpStr[str.size() - 1] == ';')
		tmpStr[str.size() - 1] = '\0';
	return (std::string(tmpStr));
}

std::vector<std::string>	ft_split(std::string const & str) {
	std::stringstream ss;
	std::vector<std::string> vect;
	std::string tmpStr;

	ss << str;
	while (ss >> tmpStr) {
		vect.push_back(tmpStr);
	}
	return (vect);
}