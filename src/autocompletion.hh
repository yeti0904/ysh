#pragma once
#include "_components.hh"

namespace AutoCompletion {
	void   Init(std::vector <std::string> executables);
	char** Completion(const char* text, int start, int end);
	char*  Generator(const char* text, int state);
}