#ifndef DRIVER_HPP
# define DRIVER_HPP

#include <map>
#include <vector>

#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#include "../Parser/ConfigParser.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"

#define READ 0
#define WRITE 1

typedef struct s_client {
	std::string host;
	std::size_t port;
	int status;
} t_client;

class Driver {
	public:
		Driver(std::list<ServerData*> &Servers);
		~Driver();
		
		void	pollCycle(void);

	private:
		//Singleton design pattern
		Driver(void) {}
		Driver(const Driver&); //don't implement
		Driver& operator=(const Driver&); //don't implement
	
		int				_servSocket(int port, std::string host);

		void				_makeFds(struct pollfd* fds);
		void				_closeClient(struct pollfd &fd);
		void				_closeRequest(int fd);
		void				_addClient(struct pollfd &fd);
		void				_readClient(struct pollfd &fd);
		void				_sendResponse(struct pollfd &fd);
	
		std::vector<int>		_socket;
		std::map<int, std::vector<ServerData*> >	_server;
		std::map<int, t_client>		_client;
	
		std::map<int, Request*>		_request;
		std::map<int, Response*>	_response;
};

#endif
