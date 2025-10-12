#include "SmflmException.h"
#include <sstream>

SmflmException::SmflmException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* SmflmException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* SmflmException::GetType() const noexcept
{
	return "Smflm Exception";
}

int SmflmException::GetLine() const noexcept
{
	return line;
}

const std::string& SmflmException::GetFile() const noexcept
{
	return file;
}

std::string SmflmException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[Line]" << line;
	return oss.str();
}