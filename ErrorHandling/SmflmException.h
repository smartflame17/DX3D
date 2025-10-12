#pragma once
#include <exception>
#include <string>

class SmflmException : public std::exception {
public:
	SmflmException(int line, const char* file) noexcept;
	const char* what() const noexcept override;		// implementation of what method in std::exception
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;		// buffer to save info after sstream dying beyond method scope end
										// needs to be mutable to allow modification from const method
};