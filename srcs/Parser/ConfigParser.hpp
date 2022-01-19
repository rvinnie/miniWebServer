#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "parser.hpp"
# include "ServerData.hpp"

class ConfigParser {

private:
	std::list<ServerData *>		*_servers;
	int							_countServers;
	//
	std::vector<std::string>	*_allServerNames;
	t_config_data				*_data;
	std::string					_path;

	void						fillData(void);
	void						checkValid(void);
	void						createNextServer(int *start);
	bool						isSpecialWord(std::string word);
	void						checkDuplicateServers(void);


	ConfigParser(void) {}
	ConfigParser(ConfigParser const &);
	ConfigParser &	operator=(ConfigParser const &);

public:
	ConfigParser(std::string path);
	~ConfigParser(void);
	void						launchConfig(void);
	void						printInfo(void) const; // don't forget to delete later
	std::list<ServerData *>		&getServers(void) const;
	int							getCountServers(void) const;
};

#endif
