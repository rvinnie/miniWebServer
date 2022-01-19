#ifndef WRONGCONFIGEXCEPTION_HPP
# define WRONGCONFIGEXCEPTION_HPP

# include "parser.hpp"

class WrongConfigException : public std::exception {

private:
	std::string	_info;
public:

	WrongConfigException(std::string info) throw();
	WrongConfigException(void) throw();
	virtual ~WrongConfigException(void) throw();
	const char	*what(void) const throw();
	std::string	getInfo(void) throw();
};

#endif
