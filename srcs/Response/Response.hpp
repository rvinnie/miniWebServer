#ifndef RESPONSE_HPP
# define RESPONSE_HPP


#include <sstream>
#include <ctime>
#include <cstdlib>
#include <dirent.h>

#include "../Handler/HandlerException.hpp"
#include "../Request/Request.hpp"
#include "../Parser/ServerData.hpp"

# include "../Parser/Location.hpp" 
# include "../Parser/ConfigParser.hpp" 

#define DONE	0
#define HEADER 1
#define	BODY 2

class Response {
	public:
		Response();
		~Response();
    
    	int   	getChunk(char* buff);
	
		void	setCode(int status);
		int		getCode( void );
		void	setUrl(std::string path);
		void	setLocation(std::string location);
		void	form(Request&);

     	bool	isComplete(void) const;
  
	private:
		//Singleton design pattern
		Response(const Response&); //don't implement
		Response& operator=(const Response&); //don't implement
		
		int			_code;
  		int			_status;
		std::string	_url;
		std::string	_location;
		bool		_autoindex;

	
		std::stringstream _header;
		std::stringstream _body;
	
		std::string	_statusText();
		std::string	_headerTime(void);
		std::string	_bakeCookie(void);


		typedef std::map<std::string, std::string> mimeType;
		static mimeType _mimeType;
		static mimeType initMimeType() {
			mimeType mt;
			mt["html"] = "text/html";
			mt["txt"] = "text/plain";
			mt["css"] = "text/css";
			mt["gif"] = "image/gif";
			mt["ico"] =  "image/x-icon";
			mt["jpeg"] =  "image/jpeg";
			mt["webp"] =  "image/webp";
			mt["jpg"] = "image/jpeg";
			mt["png"] = "image/png";
			mt["mpeg"] = "audio/mpeg";
			mt["mp3"] = "audio/mpeg";
			mt["wav"] =  "audio/wave";
			mt["mp4"] = "video/mp4";
			mt["exe"] = "application/octet-stream";
			mt["epub"] = "application/epub+zip";
			mt["zip"] = "application/zip";
			mt["pdf"] = "application/pdf";
			return mt;
		}
		std::stringstream *generateAutoindex(std::string rootPath, std::string path);
		std::string methodHandler(Request *request);
};

#endif
