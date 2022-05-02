#pragma once
#include "_components.hh"

// app class
class App;

// command types and stuff
struct Command;
typedef std::unordered_map <std::string, Command> CommandMap;
typedef uint8_t (*CommandFunction)(std::vector <std::string>, CommandMap, App);

struct Command {
	bool                      registered;
	CommandFunction           function;
	std::vector <std::string> helpItems;
};

// builtin commands
namespace BuiltInCommands {
	uint8_t Help(std::vector <std::string> argv, CommandMap commands, App app);
	uint8_t Exit(std::vector <std::string> argv, CommandMap commands, App app);
	uint8_t Set(std::vector <std::string> argv, CommandMap commands, App app);
	uint8_t Cd(std::vector <std::string> argv, CommandMap commands, App app);
	//uint8_t Strcmp(std::vector <std::string> argv, CommandMap commands); // gone
	uint8_t If(std::vector <std::string> argv, CommandMap commands, App app);
	uint8_t Invert(std::vector <std::string> argv, CommandMap commands, App app);
	uint8_t ListExecutables(std::vector <std::string> argv, CommandMap commands, App app);
}