#pragma once
#include "_components.hh"

// command types and stuff
struct Command;
typedef std::unordered_map <std::string, Command> CommandMap;
typedef uint8_t (*CommandFunction)(std::vector <std::string>, CommandMap);

struct Command {
	bool                      registered;
	CommandFunction           function;
	std::vector <std::string> helpItems;
};

// builtin commands
namespace BuiltInCommands {
	uint8_t Help(std::vector <std::string> argv, CommandMap commands);
	uint8_t Exit(std::vector <std::string> argv, CommandMap commands);
}