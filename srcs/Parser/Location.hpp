#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "parser.hpp"

class Location {

private:
	bool						_isGet;
	bool						_isPut;
	bool						_isPost;
	bool						_isDelete;
	bool						_autoindex;
	int							_maxBodySize;
	int							_type;
	std::string					_root;
	std::string					_upload;
	std::string					_index;
	std::string					_cgi;
	std::string					_scriptFilename;
	std::string					_redirect;
	std::string					_autoindexDir;
	std::vector<std::string>	*_paths;
	//
	int							_start;
	int							_end;
	t_config_data				*_data;

	void						fillPaths(void);
	void						fillInfo(void);
	Location(void) {};
	Location(Location const &);
	Location & operator=(Location const &);

public:
	Location(int start, int end, t_config_data *data);
	~Location(void);
	void						launchLocation(void);
	void						printInfo(void) const; // don't forget to delete
	bool						getIsGet(void) const;
	bool						getIsPut(void) const;
	bool						getIsPost(void) const;
	bool						getIsDelete(void) const;
	bool						getAutoindex(void) const;
	std::string					getAutoindexDir(void) const;
	int							getType(void) const;
	int							getMaxBodySize(void) const;
	std::string					getRoot(void) const;
	std::string					getIndex(void) const;
	std::string					getCgi(void) const;
	std::string					getUpload(void) const;
	std::string					getRedirect(void) const;
	std::vector<std::string>	getPaths(void) const;
	void						setPath(std::string path);
	void						setAutoindexDir(std::string dir);
	void						setPaths(std::vector<std::string> paths);
	
};


#endif
