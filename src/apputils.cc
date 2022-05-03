#include "app.hh"
#include "lexer.hh"
#include "fs.hh"

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

	// split these tokens into commands
	std::vector <std::vector <Lexer::Token>> commands;
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i].type == Lexer::TokenType::Command) {
			std::vector <Lexer::Token> command;
			size_t j = i;
			for (; (j < tokens.size()) && (tokens[j].type != Lexer::TokenType::EndOfArguments); ++j) {
				switch (tokens[j].type) {
					case Lexer::TokenType::Command:
					case Lexer::TokenType::RedirectOutput:
					case Lexer::TokenType::Argument: {
						command.push_back(tokens[j]);
						break;
					}
					default: break;
				}
			}
			i = j;
			commands.push_back(command);
		}
	}

	if (options.dontExecute) {
		return;
	}

	// execute all commands
	for (size_t i = 0; i < commands.size(); ++i) {
		ExecuteTokens(commands[i]);
	}
}

void App::ExecuteTokens(std::vector <Lexer::Token> tokens) {
	commandArgv = {};
	size_t i = 0;
	for (; 
		(i < tokens.size()) &&
		(tokens[i].type != Lexer::TokenType::EndOfArguments) &&
		(tokens[i].type != Lexer::TokenType::RedirectOutput); ++i
	){
		//printf("Loop: token %d is %s\n", (int) i, Lexer::TokenToString(tokens[i]).c_str());
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
		uint8_t ret = commands[commandArgv[0]].function(commandArgv, commands, *this);
		if (setenv("?", std::to_string(ret).c_str(), 1) == -1) {
			perror("[ERROR] setenv failed");
			exit(1);
		}
		return;
	}

	pid_t pid = fork();
	if (pid == 0) { // child
		int redirectToFD; // file descriptor of redirectTo
		if (tokens[i].type == Lexer::TokenType::RedirectOutput) {
			// we are redirecting the output to some file descripter
			bool        redirectAll = true;
			int         redirectFrom;
			std::string redirectTo;
			if (tokens[i].content.length() != 0) {
				// the user has given us a file descriptor to redirect output from
				redirectAll  = false;
				redirectFrom = std::stoi(tokens[i].content); // the lexer has already checked if the token's content is an integer
			}

			// get where we need to redirect output to
			if ((i == tokens.size() - 1) || (tokens[i + 1].type != Lexer::TokenType::Argument)) {
				// the user didnt tell the shell where to output to
				fputs("[ERROR] location for redirecting output to not given\n", stderr);
				exit(1);
			}
			redirectTo = tokens[i + 1].content;

			// create file if it doesnt exist
			if (!FS::File::Exists(redirectTo)) {
				FS::File::Create(redirectTo);
			}

			// get file descriptor
			redirectToFD = open(redirectTo.c_str(), 0);
			if (redirectToFD < 0) {
				perror("[ERROR] open failed");
				exit(1);
			}

			// set file descriptors
			if (redirectAll) {
				for (size_t i = 0; i <= 2; ++i) {
					if (dup2(i, redirectToFD) == -1) {
						perror("[ERROR] dup2 failed");
						exit(1);
					}
				}
			}
			else {
				if (dup2(redirectFrom, redirectToFD) == -1) {
					perror("[ERROR] dup2 failed");
					exit(1);
				}
			}
		}

		if (execvp(tokens[0].content.c_str(), (char**)commandArgvRaw.data()) == -1) {
			perror("[ERROR] Command execution failed");
		}

		if (tokens[i].type == Lexer::TokenType::RedirectOutput) {
			if (close(redirectToFD) == -1) {
				perror("[ERROR] close failed");
				exit(1);
			}
		}

		exit(0);
	}
	else if (pid == -1) { // error
		perror("[ERROR] fork failed");
		exit(1);
	}
	else { // parent
		int status;
		pid = wait(&status);

		// get return value
		if (WIFEXITED(status)) {
			uint8_t retValue = WEXITSTATUS(status);
			if (setenv("?", std::to_string(retValue).c_str(), 1) == -1) {
				perror("[ERROR] setenv failed");
			}
		}
		else {
			if (setenv("?", "255", 1) == -1) {
				perror("[ERROR] setenv failed");
			}
		}
	}
}