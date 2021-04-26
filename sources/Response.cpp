#include "Response.hpp"

////////////////////
// Coplien's
////////////////////
Response::Response(Request *req, int socket)
{
	this->_location = req->getSelectedLocation();
	this->_req = req;
	this->_socket = socket;

	_httpVersion = "HTTP/1.1";

}

Response::Response( Response const & src )
{
	*this = src;
	return ;
}

Response::~Response( void )
{
	return ;
}

Response & Response::operator=( Response const & rhs)
{
	(void)rhs;
	return ( *this );
}


////////////////////
// METHODS
////////////////////
void	Response::send()
{
	write(this->_socket, this->_response.c_str() , this->_response.length());	//to protect
}

void	Response::buildResponse()
{
	std::string		requestMethod = this->_req->getMethod();

	if (requestMethod == "GET")
		this->processGet();
	else if (requestMethod == "POST")
		this->processPost();

	this->buildHeader();
	this->_response = this->_header + this->_body;
}

void	Response::buildHeader()
{
	// Minimal header accepted :
	// HTTP/1.1 200 OK
	// Content-Type: text/html
	// Content-Length: 0

	std::ostringstream header;
	header << this->_httpVersion << " " << this->_responseCode << " " << this->_responseCodeMessage << "\n";
	header << "Content-Type: " << this->_contentType << "\n";
	header << "Content-Length: " << this->_body.size(); 

	header << "\n\n";						//End of header
	this->_header = header.str();
}


////////////////////
// GET
////////////////////
void	Response::processGet()
{

	std::string		content;

	std::string		root = "files/www";			// Change to config->location->root
	std::string		auto_index= "on";			// Change to config->location->auto_index
	std::string 	index_page = "/index.html";	// Change to index in conf 

	std::string 	target;
	// Directory Request
	if (this->isDirectory())
	{
		if(!this->isIndexPagePresent() && auto_index == "on" && this->autoIndexResponse())  //autoIndexRequest return true on success
			return ;
		else if (this->isIndexPagePresent())
			this->_req->updateTarget(this->_req->getTarget() + index_page);
	}
	
	//Here comes the block where you check the file ext and define content_type
	this->_contentType = this->getContentType(this->_req->getTarget());

	// Check if the file can be open and create response
	std::ifstream 	f(this->_req->getAbsoluteTargetPath().c_str()); // open file
	if (f.good())
	{
		this->setHeaders(200, "OK", this->_contentType);
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		this->_body = str;
	}
	else
	{
		this->setHeaders(404, "FILE_NOT_FOUND", "text/html");
		std::ifstream error_page("files/default_pages/custom_404.html");
		std::string str((std::istreambuf_iterator<char>(error_page)), std::istreambuf_iterator<char>());
		this->_body = str;
	}

	f.close();
}


////////////////////
// POST
////////////////////
void	Response::processPost()
{

}


////////////////////
// AUTO INDEX
////////////////////
bool		Response::autoIndexResponse()
{
	std::string		auto_index = "on";						// Change to config->location->auto_index

	DIR *directory;
	struct dirent *dircontent;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
	{
		setHeaders(200, "OK", "text/html");

		std::ifstream content_start("files/default_pages/auto_index_start.html");
		std::ostringstream content;
		std::ifstream content_end("files/default_pages/auto_index_end.html");

		content << std::string((std::istreambuf_iterator<char>(content_start)), std::istreambuf_iterator<char>());

		content << "<h1>Directory : " << this->_req->getUrlTargetPath() << "</h1>";
		content << "<ul>" << std::endl;

		while((dircontent = readdir(directory)))
		{
				content << "<li>" << "<a href='" << this->_req->getUrlTargetPath();
				if (this->_req->getTarget().back() != '/')			// this ensure folder path to have '/' when traget is not root
					content << '/';
				content << dircontent->d_name <<"'>";
				content << dircontent->d_name << "</a>" << "</li>" << std::endl;
		}
		content << "</ul>" << std::endl;
		
		content << std::string((std::istreambuf_iterator<char>(content_end)), std::istreambuf_iterator<char>());
		this->_body = content.str();
		return true;
	}

	return false;
}


////////////////////
// UTILS
////////////////////
std::string	Response::getIndexPath()
{
	std::string 	index_page = "index.html";	// Change to index in conf 

	if (this->_req->getTarget().back() != '/')
		return(this->_req->getTarget() + "/" + index_page); 
	else
		return (this->_req->getTarget() + index_page);
}

bool		Response::isIndexPagePresent()
{
	std::string 	index_page = "index.html";	// Change to index in conf 

	std::string		target;

	if (index_page.empty())						//If no index in conf file
		return false;

	target = this->_req->getAbsoluteTargetPath() + "/" + index_page;

	std::ifstream 	f(target.c_str());

	return (f.good());
}

bool		Response::isDirectory()
{
	DIR *directory;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
		return true;
	return false;
}

std::string	Response::getContentType(std::string target)
{
	//kudos to the guy who wrote those variables
	std::string extension[67] = {"php", "aac", "abw", "arc", "avi", "azw", "bin", "bz", "bz2", "csh", "css", "csv", "doc", "docsx", "eot", "epub", "gif", "htm", "html", "ico",
	"ics", "jar", "jpeg", "jpg", "js", "json", "mid", "midi", "mpeg", "mpkg", "odp", "ods", "odt", "oga", "ogv", "ogx", "otf", "png", "pdf", "ppt", "pptx", "rar", "rtf", "sh"
	"svg", "swf", "tar", "tif", "tiff", "ts", "ttf", "vsd", "wav", "weba", "webm", "webp", "woff" ,"woff2", "xhtml", "xls", "xlsx","xml", "xul", "zip", "3gp", "3g2", "7z"};
	std::string content_type[67] = {"text/html", "audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", "application/octet-stream", "application/x-bzip",
	"application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/vnd.ms-fontobject",
	"application/epub+zip", "image/gif", "text/html", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "image/jpeg" ,"application/javascript", "application/json",
	"audio/midi","audio/midi", "video/mpeg", "	application/vnd.apple.installer+xml", "application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
	"application/x-rar-compressed", "application/rtf", "application/x-sh", "image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "image/tiff", "application/typescript", "font/ttf", "application/vnd.visio", "audio/x-wav", "audio/webm", "video/webm", "image/webp", "font/woff", "font/woff2", "application/xhtml+xml", "application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
	"application/xml", "application/vnd.mozilla.xul+xml", "application/zip", "video/3gpp", "video/3gpp2", "application/x-7z-compressed"};

	int i = target.size() - 1;
	while (i >= 0 && target[i] != '.')
		i--;

	std::string ext = target.substr(i + 1, target.size() - 1);

	int j = 0;
	while (j < 67)
	{
		if (extension[j] == ext)
			return (content_type[j]);
		j++;
	}

	return ("text/plain");
}

////////////////////
// GETTERS / SETTERS
////////////////////
void	Response::setHeaders(int responseCode, std::string responseCodeMessage, std::string contentType)
{
	this->_responseCode = responseCode;
	this->_responseCodeMessage = responseCodeMessage;
	this->_contentType = contentType;
}

std::string Response::getResponse()
{
	return (this->_response);
}

std::ostream &	operator<<(std::ostream & o, Response & rhs)
{
	o << "In this response we have :\n";
	o << "Index path : " << rhs.getIndexPath() << "\n";
	// o << "Response : " << rhs.getResponse() << "\n";

	return ( o );
}
