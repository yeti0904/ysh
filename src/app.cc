#include "app.hh"
#include "util.hh"
#include "lexer.hh"
#include "constants.hh"
#include "commands.hh"
#include "fs.hh"

App::App(int argc, char** argv) {
	// set default options
	options.showTokens = false;

	// create directories and files
	if (!FS::Directory::Exists(std::string(getenv("HOME")) + "/.config")) {
		FS::Directory::Create(std::string(getenv("HOME")) + "/.config");
	}
	if (!FS::Directory::Exists(std::string(getenv("HOME")) + "/.config/ysh2")) {
		FS::Directory::Create(std::string(getenv("HOME")) + "/.config/ysh2");
	}
	if (!FS::File::Exists(std::string(getenv("HOME")) + "/.config/ysh2/init.lua")) {
		FS::File::Create(std::string(getenv("HOME")) + "/.config/ysh2/init.lua");
		FS::File::Write(std::string(getenv("HOME")) + "/.config/ysh2/init.lua",
			"-- YSH init script\n"
			"YSH_SetEnv(\"YSH_PROMPT\", \"$ \")\n"
		);
	}

	// loop through arguments
	for (int i = 1; i < argc; ++i) {
		std::string currentArg = argv[i];
		if ((currentArg == "-v") || (currentArg == "--version")) {
			printf("%s %s\n", APP_NAME, APP_VERSION);
			exit(0);
		}
		if ((currentArg == "-t") || (currentArg == "--tokens")) {
			options.showTokens = true;
		}
	}

	// set variables
	run = true;

	// set env variables
	if (getenv("YSH_PROMPT") == nullptr) {
		setenv("YSH_PROMPT", "$ ", 0);
	}
	setenv("SHELL", argv[0], 1);

	// register commands
	RegisterCommand("help", BuiltInCommands::Help, {
		"help [command]",
		"  shows you how to use a command"
	});
	RegisterCommand("exit", BuiltInCommands::Exit, {
		"exit",
		"  makes ysh quit"
	});
}

void App::Update() {
	rawInput = readline(Util::Escape(getenv("YSH_PROMPT")).c_str());
	if (rawInput == nullptr) {
		puts("readline returned NULL");
		while (true) {}
	}

	input = rawInput;
	free(rawInput);

	// lex input
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
	for (size_t i = 0; i < tokens.size(); ++i) {
		commandArgvRaw.push_back(commandArgv[i].c_str());
	}
	commandArgvRaw.push_back(nullptr);

	if (commands[commandArgv[0]].registered) {
		commands[commandArgv[0]].function(commandArgv, commands);
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
	}
	else { // parent
		int status;
		pid = wait(&status);
	}
}

App::~App() {

}