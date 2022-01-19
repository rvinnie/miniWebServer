#include  "WrongConfigException.hpp"

WrongConfigException::WrongConfigException(std::string info) throw() : _info("WrongConfigException: " + info) {}

WrongConfigException::WrongConfigException(void) throw() : _info("wrong config") {}

WrongConfigException::~WrongConfigException(void) throw() {}

std::string WrongConfigException::getInfo(void) throw() { return(this->_info); };

const char *WrongConfigException::what(void) const throw() {return (this->_info.c_str());}
