#include "app.hh"

void App::RegisterCommand(std::string name, CommandFunction function, std::vector <std::string> helpItems) {
	Command command;
	command.registered = true;
	command.function   = function;
	command.helpItems  = helpItems;

	commands[name] = command;
}