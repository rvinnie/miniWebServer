#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../Parser/ServerData.hpp"

#define BUFF_SIZE 1024

enum Status {
			FIRST_LINE,
			HEADERS,
			BODY,
			COMPLETE,
			ERROR
		};

enum chunkStatus {
			SIZE,
			CONTENT
		};

class Request {
	private:

		Request(const Request&);
		Request& operator=(const Request&);
		void	setServer( void );

		std::string							_version; //HTTP/1.1 (maybe 1.0)
		std::string 						_buffer;
		Status		 						_status;
		std::string							_method;
		std::string 						_body;
		size_t								_chunkSize;
		chunkStatus							_chunkStatus;
		size_t								_error;

		size_t								_length;
		std::map<std::string, std::string>	_headers;

		std::string							_urn;
		std::string							_path;
		std::string							_query; //..?query (key1=value&key2=value or key1=value1;key2=value2)

		std::vector<ServerData*>			_serverVector;

		ServerData*							_server;
		std::string 						_rootPath; // path to root of project

		void	findRootPath();

	public:

		Request(void) {}

		// Request(ServerData*);
		Request(std::vector<ServerData*>);

		
		~Request();
		std::string getVersion();
		std::string getBuffer();
		Status		getStatus();
		size_t		getLength();
		size_t		getError();
		ServerData* getServer();
		std::string getMethod(); //GET, PUT, DELETE; !!! error -> 405 ошибка возвращается в parseFirstLine
		std::string getUrn();
		std::string getBody();
		std::string getPath();
		std::string getRootPath();
		std::map<std::string, std::string> getHeaders();
		void		checkHeaders( void );
		
		std::string	getHeader(std::string); //ex: getHeader("Cookie");
		std::string	getQuery();

		bool	isVersionValid(); //is it HTTP/1.1; error -> 400
		bool	isBodyLarge(); //is body bigger than the limit -> 413

		void	addBuff(char*, int); 

		int 	parse(std::string &buf);

		int		parseFirstLine( void );
		int		parseHeaders( void );
		int		parseBody( void );
		int		parseChunkedBody( void );

		void	setComplete(void);
		bool	isComplete(void) const;	
};

#endif
