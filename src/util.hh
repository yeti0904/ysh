#pragma once
#include "_components.hh"

namespace Util {
	std::string Escape(std::string src);
	std::string GetWorkingDirectory();
	std::string StringReplaceAll(std::string src, std::string replaceWhat, std::string replaceWith);
	std::string StringReplaceOne(std::string src, std::string replaceWhat, std::string replaceWith);
}