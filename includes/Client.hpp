#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>
# include "Logger.hpp"

class Client
{
private:

	long		_socket;
	int			_servNb;
	std::string	_buff;
	bool		_finishRead;
	bool		_finishWrite;

public:

	Client( void );									//default constructor
	Client( long socket, int servNb );				//constructor
	Client( Client const & src);  					//copy
	~Client( void );								//destructor
	Client & operator=( Client const & rhs );		//overload operator =

	int			myRecv( void );
	bool		checkReadState( void );
	void		deleteBuff( void );
	int			getSocket( void );
	int			getServerNb( void );
	bool		getFinishRead( void );
	bool		getFinishWrite( void );
	std::string	getBuffer( void );
	void		setFinishWrite( bool finishWrite );
	void		setFinishRead( bool finishRead );


};

std::ostream &	operator<<(std::ostream & o, Client & rhs);

#endif
