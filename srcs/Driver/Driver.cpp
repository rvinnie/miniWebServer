#include "Driver.hpp"

Driver::Driver(std::list<ServerData*> &Servers) {
	signal(SIGPIPE, SIG_IGN);
	for (std::list<ServerData*>::iterator it = Servers.begin(); it != Servers.end(); it++) {
		int sockfd = _servSocket((*it)->getPort(), (*it)->getHost());
		if (sockfd < 0) {
			for (std::map<int, std::vector<ServerData*> >::iterator it2 = _server.begin(); it2 != _server.end(); ++it2) {
				std::vector<ServerData*> curr = (*it2).second;
				bool found = false;
				for (size_t i = 0; i < curr.size(); i++) {
					if (curr[i]->getHost() == (*it)->getHost() && curr[i]->getPort() == (*it)->getPort()) {
						it2->second.push_back(*it);
						found = true;
						break ;
					}
				}
				if (found)
					break ;
			}
			continue;
		}
		_socket.push_back(sockfd);
		std::cout << "Server listeninig at host "
			<< (*it)->getHost()
			<< " port " << (*it)->getPort() << std::endl;
		_server[sockfd].push_back(*it);
	}
}

Driver::~Driver() {
	for(std::map<int, Request*>::iterator it = _request.begin(); it != _request.end(); ++it)
		delete (*it).second;
	for(std::map<int, Response*>::iterator it = _response.begin(); it != _response.end(); ++it)
		delete (*it).second;
	for(std::map<int, t_client>::iterator it = _client.begin(); it != _client.end(); ++it) {
		 close((*it).first);
	}
	for(std::vector<int>::iterator it = _socket.begin(); it != _socket.end(); ++it) {
		 close(*it);
	}
}

int	Driver::_servSocket(int port, std::string host) {
	struct sockaddr_in saddr;
	std::memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = (!host.size()) ? htonl(INADDR_ANY) : inet_addr(host.c_str());
	saddr.sin_port        = htons(port);

	char buff[256];

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		strerror_r(errno, buff, 256);
		throw std::runtime_error(buff);
	}
	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
		strerror_r(errno, buff, 256);
		throw std::runtime_error(buff);
	}
	if (bind(listenfd, (struct sockaddr *) &saddr, sizeof(saddr))) {
		if (errno == 48)
			return -1;
		strerror_r(errno, buff, 256);
		throw std::runtime_error(buff);
	}
	if (listen(listenfd, 128)) {
		strerror_r(errno, buff, 256);
		throw std::runtime_error(buff);
	}
	return listenfd;
}

void Driver::_makeFds(struct pollfd *fds) {
	std::size_t i;
	for (i = 0; i < _socket.size(); ++i) {
		fds[i].fd = _socket[i];
		fds[i].events = POLLIN;
		fds[i].revents = 0;
	}
	for (std::map<int, t_client>::iterator it = _client.begin(); it != _client.end(); ++it) {
		fds[i].fd = it->first;
		fds[i].revents = 0;
		if (it->second.status == READ)
			fds[i].events = POLLIN;
		else
			fds[i].events = POLLOUT;
		++i;
	}
}

void	Driver::pollCycle() {
	std::size_t nlisten = _socket.size();

	while(true) {
		std::size_t nfds = _socket.size() + _client.size();
		struct pollfd fds[nfds];
		_makeFds(&fds[0]);
		int r = poll(fds, nfds, -1);
		if (r < 0) {
			char buff[256];
			strerror_r(errno, buff, 256);
			throw std::runtime_error(buff);
		}
		if (r > 0) {
			for (std::size_t i = 0; i < nfds; i++) {
				if (fds[i].revents == POLLIN) {
					if (i < nlisten)
						_addClient(fds[i]);
					else
						_readClient(fds[i]);
				}
				else if (fds[i].revents == POLLOUT)
					_sendResponse(fds[i]);
				else if (fds[i].revents & (POLLERR|POLLHUP))
					_closeClient(fds[i]);
				else if (fds[i].revents == POLLNVAL)
					std::cerr << "File descriptor " << fds[i].fd << " is invalid" << std::endl;
			}
		}
	}

}

void	Driver::_closeClient(struct pollfd &fd) {
	std::cout << "Closing connection at host " << _server[fd.fd][0]->getHost()
			<< " port " << _server[fd.fd][0]->getPort()
			<< " with client at host " << _client[fd.fd].host
			<< " port " << _client[fd.fd].port << std::endl;
	close(fd.fd);
	_closeRequest(fd.fd);
	_server.erase(fd.fd);
	_client.erase(fd.fd);
	fd.fd = -1;
	fd.revents = 0;
}

void	Driver::_closeRequest(int fd) {
	delete _request[fd];
	_request.erase(fd);
	delete _response[fd];
	_response.erase(fd);
}

void	Driver::_addClient(struct pollfd &fd) {
	struct sockaddr_in servaddr = {};
  	socklen_t servlen = sizeof(servaddr);
  	getsockname(fd.fd, reinterpret_cast<sockaddr *>(&servaddr), &servlen);

	struct sockaddr_in  cliaddr = {};
	socklen_t	clilen = sizeof(cliaddr);
	int new_socket = accept(fd.fd, reinterpret_cast<struct sockaddr *>(&cliaddr), &clilen);
	if (new_socket < 0) {
		std::cerr << "\rPoll: Accept error" << std::endl;
		return;
	}
	fcntl(new_socket, F_SETFL, O_NONBLOCK);
	_server[new_socket] = _server[fd.fd];
	_request[new_socket] = new Request(_server[fd.fd]);
	_response[new_socket] = new Response();
	t_client client;
	client.host = inet_ntoa(*reinterpret_cast<const in_addr *>(&cliaddr.sin_addr.s_addr));
	client.port = ntohs(cliaddr.sin_port);
	client.status = READ;
	_client[new_socket] = client;
	std::cout << "Client host " << client.host
		<< " port " << client.port
		<< " connected to host " << inet_ntoa(*reinterpret_cast<const in_addr *>(&servaddr.sin_addr.s_addr))
		<< " port " << ntohs(servaddr.sin_port)
		<< std::endl;
}

void	Driver::_readClient(struct pollfd &fd) {
	char buff[BUFF_SIZE];
	bzero(buff, BUFF_SIZE);
	int r = recv(fd.fd, buff, BUFF_SIZE - 1, 0);
	if (r <= 0) {
		_closeClient(fd);
		if (r < 0)
			std::cerr << "\rPoll: recv error" << std::endl;
		return;
	}
	buff[r] ='\0';
	std::string b(buff);
	int code = _request[fd.fd]->parse(b);
   	if (code && _response[fd.fd]->getCode() == 200)
		_response[fd.fd]->setCode(code);
	if (_request[fd.fd]->isComplete()) {
		_response[fd.fd]->form(*_request[fd.fd]);
		fd.events = POLLOUT;
		fd.revents = 0;
		_client[fd.fd].status = WRITE;
	}
}

void	Driver::_sendResponse(struct pollfd &fd) {
	if (_response[fd.fd]->isComplete()) {
		_client[fd.fd].status = READ;
		if (_response[fd.fd]->getCode() == 400)
			_closeClient(fd);
		else {
			_closeRequest(fd.fd);
			_request[fd.fd] = new Request(_server[fd.fd]);
			_response[fd.fd] = new Response();
		}
		return;
	}
	char buff[BUFF_SIZE];
	bzero(buff, BUFF_SIZE);
	std::size_t size = _response[fd.fd]->getChunk(buff);
	int r = send(fd.fd, buff, size, 0);
	if (r < 0) {
		_closeClient(fd);
	}
}
