#include "handler.hpp"
#include "../Parser/ServerData.hpp"
#include "../Parser/Location.hpp"


#include <unistd.h>

std::string getContentLen(Request *request) {
	if (request->getHeader("content-length").size())
		return (request->getHeader("content-length"));
	int a = request->getBody().size();
	std::stringstream ss;
	ss << a;
	std::string str = ss.str();
	return (str);
}

std::string cgiHandler(Request *request, Location *location) {

	std::string	BODY = request->getBody();
	std::map<std::string, std::string> headers = request->getHeaders();

	std::vector<std::string> preenv;
	preenv.push_back("SERVER_SOFTWARE=NONAME_TEAM");
	preenv.push_back("SERVER_NAME=localhost");
	preenv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	preenv.push_back("SERVER_PROTOCOL=HTTP/1.1");
	preenv.push_back("SERVER_PORT=8080");
	preenv.push_back("GENERIC-RESPONSE=RESPONSE-BODY");
	preenv.push_back(std::string("QUERY_STRING=") + request->getQuery());
	preenv.push_back(std::string("CONTENT_LENGTH=") + getContentLen(request));
	preenv.push_back(std::string("CONTENT_TYPE=") + request->getHeader("content-type"));
	preenv.push_back(std::string("PATH_INFO=") + request->getPath());
	preenv.push_back(std::string("REQUEST_METHOD=") + request->getMethod());
	preenv.push_back(std::string("REQUEST_URI=") + request->getPath());
	preenv.push_back(std::string("SCRIPT_FILENAME=") + request->getRootPath() + location->getCgi());
	preenv.push_back(std::string("SCRIPT_NAME=") + location->getCgi());
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
		std::string header = it->first;
		std::transform(header.begin(), header.end(), header.begin(), ::toupper);
		for (size_t pos = header.find('-'); pos != std::string::npos; pos = header.find('-'))
			header.replace(pos, 1, "_");
        preenv.push_back("HTTP_" + header + "=" + it->second);
	}
	const char	*env[preenv.size() + 1];
	env[preenv.size()] = NULL;
	for (size_t i = 0; i < preenv.size(); ++i)
		env[i] = preenv[i].c_str();
	std::string filename = request->getRootPath() + location->getCgi();

	char *argv[2] = {const_cast<char *>(filename.c_str()), NULL };
	std::string page;
	FILE	*inFile = tmpfile();
	FILE	*outFile = tmpfile();
	if (inFile == NULL || outFile == NULL)
		throw HandlerException("500");

	long	fdIn = fileno(inFile);
	long	fdOut = fileno(outFile);
	write(fdIn, BODY.c_str(), BODY.size());
	lseek(fdIn, 0, SEEK_SET);

	int pid = fork();
	if (pid < 0)
		throw HandlerException("500");
	else if (pid == 0) {
		dup2(fdIn, 0);
		dup2(fdOut, 1);		
		int ret = execve(filename.c_str(), argv, const_cast<char *const *>(env));
		close(fdIn);
		close(fdOut);
		exit(ret);
	} else {
		int res;
		waitpid(-1, &res, 0);
		lseek(fdOut, 0, SEEK_SET);
		const int size = 65536;
		char buff[size] = {0};
		int count = 1;
		while (count > 0) {
			memset(buff, 0, size);
			count = read(fdOut, &buff, size - 1);
			page += buff;
		}
	}
	close(fdIn);
	close(fdOut);
	return (page);
}
