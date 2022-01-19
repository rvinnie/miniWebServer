#include "Parser/ConfigParser.hpp"
#include "Parser/WrongConfigException.hpp"
#include "Driver/Driver.hpp"

int main(int argc, char* argv[]) {
	std::string name;
	if (argc == 1)
		name = "configs/webserv.conf";
	else {
		std::string fileName = argv[1];
		name = "configs/" + fileName;
	}
	ConfigParser parser(name);
	try {
		parser.launchConfig();
	} catch (WrongConfigException & e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	try {
		Driver driver(parser.getServers());
		driver.pollCycle();
	} catch (const std::exception & e) {
		std::cerr << "DriverException: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
