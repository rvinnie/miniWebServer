#ifndef PARSER_H
# define PARSER_H

# define SERVER_NAME "webserver.ru"
# define PATH_TO_400 "400.html"
# define PATH_TO_403 "403.html"
# define PATH_TO_404 "404.html"
# define PATH_TO_405 "405.html"
# define PATH_TO_408 "408.html"
# define PATH_TO_411 "411.html"
# define PATH_TO_413 "413.html"
# define PATH_TO_418 "418.html"
# define PATH_TO_500 "500.html"
# define PATH_TO_501 "501.html"
# define PATH_TO_502 "502.html"
# define PATH_TO_505 "505.html"
# define COLOR_RED     "\x1b[31m"
# define COLOR_GREEN   "\x1b[32m"
# define COLOR_YELLOW  "\x1b[33m"
# define COLOR_BLUE    "\x1b[34m"
# define COLOR_MAGENTA "\x1b[35m"
# define COLOR_CYAN    "\x1b[36m"
# define COLOR_RESET   "\x1b[0m"

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <exception>
# include <algorithm>
# include <vector>
# include <list>
# include <map>
# include <unistd.h>
# include <sys/wait.h>
# include <cstdio>
# include "WrongConfigException.hpp"

typedef std::vector< std::vector< std::string > > t_config_data;
std::vector<std::string>	ft_split(std::string const & str);
void						checkParameter(std::string & str);
int							convertToInt(std::string & str);
std::string					removeSemi(std::string & str);

#endif
