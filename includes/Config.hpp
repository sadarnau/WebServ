/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/12 16:21:54 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/14 14:34:06 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <iostream>
# include "Logger.hpp"

class Config
{
private:

	std::ifstream						f;		// open index.html
	std::map<std::string, std::string>	fileMap;

public:

	Config( void );								//default constructor
	// Config( std::string name );					//constructor
	Config( Config const & src);  				//copy
	~Config( void );							//destructor
	Config & operator=( Config const & rhs );	//overload operator =

	void	parseFile( std::string fileName );
	void	checkFile( std::string fileName );
	void	createMap( void );
};

// template<typename K, typename V>
// void print_map(std::map<K, V> const &m)
// {
// 	for (auto it = m.cbegin(); it != m.cend(); ++it) {
// 		std::cout << "{" << (*it).first << ": " << (*it).second << "}\n";
// 	}
// };

#endif
