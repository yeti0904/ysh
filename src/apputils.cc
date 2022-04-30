#include "app.hh"
#include "lexer.hh"

void App::RegisterCommand(std::string name, CommandFunction function, std::vector <std::string> helpItems) {
	Command command;
	command.registered = true;
	command.function   = function;
	command.helpItems  = helpItems;

	commands[name] = command;
}

void App::ExecuteScript(std::string input) {
	std::vector <Lexer::Token> tokens = Lexer::Lex(input);
	if (tokens.size() == 0) return;
	if (options.showTokens) {
		for (size_t i = 0; i < tokens.size(); ++i) {
			printf("[%d] %s: %s\n", (int) i, Lexer::TokenToString(tokens[i]).c_str(), tokens[i].content.c_str());
		}
	}

	// execute it
	commandArgv = {};
	for (size_t i = 0; i < tokens.size(); ++i) {
		if ((tokens[i].type == Lexer::TokenType::Argument) && (tokens[i].content[0] == '$')) {
			char* variableContent = getenv(tokens[i].content.substr(1).c_str());
			if (variableContent == nullptr) {
				commandArgv.push_back(" ");
			}
			else {
				commandArgv.push_back(variableContent);
			}
		}
		else {
			commandArgv.push_back(tokens[i].content.c_str());
		}
	}
	commandArgvRaw = {};
	for (size_t i = 0; i < commandArgv.size(); ++i) {
		commandArgvRaw.push_back(commandArgv[i].c_str());
	}
	commandArgvRaw.push_back(nullptr);

	if (commands[commandArgv[0]].registered) {
		uint8_t ret = commands[commandArgv[0]].function(commandArgv, commands);
		if (setenv("?", std::to_string(ret).c_str(), 1) == -1) {
			perror("[ERROR] setenv failed");
			exit(1);
		}
		return;
	}

	pid_t pid = fork();
	if (pid == 0) { // child
		if (execvp(tokens[0].content.c_str(), (char**)commandArgvRaw.data()) == -1) {
			perror("[ERROR] Command execution failed");
		}
		exit(0);
	}
	else if (pid == -1) { // error
		perror("[ERROR] fork");
		exit(1);
	}
	else { // parent
		int status;
		pid = wait(&status);
	}
}