#ifndef HANDLEREXCEPTION_HPP
# define HANDLEREXCEPTION_HPP

# include "handler.hpp"

class HandlerException : public std::exception {

private:
	std::string	_info;
public:
	HandlerException(std::string info) throw();
	HandlerException(void) throw();
	virtual ~HandlerException(void) throw();
	const char	*what(void) const throw();
	std::string	getInfo(void) throw();
};

#endif
