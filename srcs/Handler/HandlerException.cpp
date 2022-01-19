#include  "HandlerException.hpp"

HandlerException::HandlerException(std::string info) throw() : _info(info) {}

HandlerException::HandlerException(void) throw() : _info("404") {}

HandlerException::~HandlerException(void) throw() {}

std::string HandlerException::getInfo(void) throw() { return(this->_info); };

const char *HandlerException::what(void) const throw() { return (this->_info.c_str()); }
