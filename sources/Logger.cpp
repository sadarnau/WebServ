#include "Logger.hpp"

Logger Logger::Log;

Logger::Logger(void) {}

Logger::~Logger(void) {
	Log.Stop();
}

const std::string Logger::_priorityNames[] = {
	"DEBUG",
	"INFO ",
	"ERROR"
};

void
Logger::Start(Priority minPriority) {
	Log._minPriority = minPriority;
	Log._fileStream.open("./log/log.txt", std::ofstream::out);
	Log._fileStream << "--------LOG----------\n";
	if (Log._fileStream.is_open())
        Log._fileStream.close();
}

void
Logger::Write(Priority priority, const std::string &col, const std::string &message, bool _isWriteStdout) {
	Log._fileStream.open("./log/log.txt", std::ofstream::app);
	if (priority >= Log._minPriority) {
		std::string output;
		output.clear();
		if (priority == DEBUG)
			output.append(BLU);
		else if (priority == INFO)
			output.append(YEL);
		else if (priority == ERROR)
			output.append(RED);
		output.append(_priorityNames[priority]);
		output.append("[");
		output.append(getTimeHMS());
		output.append("]: ");
		output.append(col);
		output.append(message);
		output.append(RESET);

		std::ostream& stream1 = Log._fileStream;
		stream1 << output;
		if (_isWriteStdout)
			std::cout << output;
	}
	if (Log._fileStream.is_open())
        Log._fileStream.close();
}

void
Logger::Error(const std::string &message) {
	if (errno != 0)
		Logger::Write(Logger::ERROR, std::string(RED), message + " -> (" + std::string(strerror(errno)) + ")", true);
	else
		Logger::Write(Logger::ERROR, std::string(RED), message, true);
}

void
Logger::Stop(void)
{
    if (Log._fileStream.is_open())
        Log._fileStream.close();
}