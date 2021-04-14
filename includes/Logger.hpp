#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <cerrno>
# include <cstring>
# include "Utils.hpp"
# include "colors.hpp"


class Logger {
	public:
		enum Priority {
            DEBUG,
            INFO,
            ERROR,
			NONE
        };
	private:
        Logger();
        Logger(const Logger &rhs);
        Logger &operator=(const Logger &rhs);

        std::ofstream _fileStream;
        Priority _minPriority;
        static const std::string _priorityNames[];
        static Logger Log;

	public:
        static void Start(Priority minPriority);
        static void Stop(void);
        static void Write(Priority priority, const std::string &col, const std::string &message, bool _isWriteStdout);
        static void Error(const std::string &message);
        ~Logger();
};

#endif