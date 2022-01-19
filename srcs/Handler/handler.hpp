#ifndef HANDLER_HPP
# define HANDLER_HPP

# include "../Parser/Location.hpp"
# include "../Parser/ServerData.hpp"
# include "../Parser/ConfigParser.hpp"
# include "../Request/Request.hpp"

# include "HandlerException.hpp"

std::string cgiHandler(Request *request, Location *location);
Location *findLocation(std::list<Location *> locations, std::string urn, Request *request);

#endif
