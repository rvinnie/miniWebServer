#ifndef SERVERDATA_HPP
# define SERVERDATA_HPP

# include "parser.hpp"
# include "Location.hpp"

class ServerData {

private:
	int							_port;
	int							_maxClientBodySize;
	std::string					_host;
	std::map<int, std::string>	_errors;
	std::vector<std::string>	*_serverNames;
	std::list<Location *>		*_locations;
	//
	int							_countLocations;
	int							_start;
	int							_end;
	t_config_data				*_data;
	std::vector<std::string>	*_allPaths;

	void						fillInfo(void);
	void						createNextLocation(int *start);
	void						checkInfo(void);
	void						addError(std::string code, std::string description);
	void						checkDuplicateLocations(std::list<Location *>::iterator & it);
	void						setLocationsRedirect(std::vector<std::string> const & what, std::string const & where);

	ServerData(void) {};
	ServerData(ServerData const &);
	ServerData & operator=(ServerData const &);

public:
	ServerData(int start, int end, t_config_data *data);
	~ServerData(void);
	void						launchServerData(void);
	std::string					getErrorPage(int code);
	void						printInfo(void) const; // don't forget to delete later
	int							getPort(void) const;
	std::string					getHost(void) const;
	std::vector<std::string>	getServerNames(void) const;
	int							getMaxBodySize(void) const;
	std::map<int, std::string>	getErrors(void) const;
	std::list<Location *>		getLocations(void) const;

};


#endif
