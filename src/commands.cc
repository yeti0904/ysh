#include "commands.hh"

uint8_t BuiltInCommands::Help(std::vector <std::string> argv, CommandMap commands) {
	if (argv.size() == 1) {
		puts("[ERROR] Help: not enough arguments");
		return 1;
	}
	if (!commands[argv[1]].registered) {
		puts("[ERROR] Help: command doesnt exist");
		return 1;
	}
	for (size_t i = 0; i < commands[argv[1]].helpItems.size(); ++i) {
		puts(commands[argv[1]].helpItems[i].c_str());
	}
	return 0;
}

uint8_t BuiltInCommands::Exit(std::vector <std::string> argv, CommandMap commands) {
	(void) argv;
	(void) commands;
	exit(0);
	return 0;
}