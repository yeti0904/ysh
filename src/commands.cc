#include "commands.hh"
#include "fs.hh"
#include "constants.hh"

uint8_t BuiltInCommands::Help(std::vector <std::string> argv, CommandMap commands) {
	if (argv.size() == 1) {
		/* Old code
		puts("[ERROR] Help: not enough arguments");
		return 1;
		*/
		puts(
			"Welcome to " APP_NAME "\n"
			"Commands:\n"
			"help, exit, set, cd, if, invert"
		);
		return 0;
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

	uint8_t exitCode = 0;

	// get exit code given by user if there is one
	if (argv.size() > 1) {
		exitCode = std::stoi(argv[1]);
	}

	exit(exitCode);
	return exitCode;
}

uint8_t BuiltInCommands::Set(std::vector <std::string> args, CommandMap commands) {
	(void) commands;
	if (args.size() < 3) {
		puts("[ERROR] Set: not enough arguments");
		return 1;
	}
	setenv(args[1].c_str(), args[2].c_str(), 1);
	return 0;
}

uint8_t BuiltInCommands::Cd(std::vector <std::string> argv, CommandMap commands) {
	(void) commands;

	if (argv.size() < 2) {
		chdir(getenv("HOME"));
		return 0;
	}

	if (FS::Directory::Exists(argv[1])) {
		chdir(argv[1].c_str());
	}
	else {
		puts("[ERROR] Cd: Directory doesn't exist");
		return 1;
	}
	return 0;
}

/*uint8_t BuiltInCommands::Strcmp(std::vector <std::string> argv, CommandMap commands) {
	(void) commands;

	if (argv.size() < 3) {
		puts("[ERROR] Strcmp: not enough arguments");
		return 1;
	}

	if (argv[1] == argv[2]) {
		return RETCODE_TRUE;
	}
	return RETCODE_FALSE;
}*/

uint8_t BuiltInCommands::If(std::vector <std::string> argv, CommandMap commands) {
	if (argv.size() < 2) {
		puts("[ERROR] If: not enough arguments");
	}

	char* commandRetValue = getenv("?");
	if (commandRetValue == nullptr) {
		printf("[ERROR] If: failed to get return value");
		return 1;
	}
	uint8_t commandRetBool = atoi(commandRetValue);

	if (commandRetBool != RETCODE_TRUE) return 0;

	std::vector <std::string> commandArgv = argv;
	commandArgv.erase(commandArgv.begin());

	std::vector <const char*> commandArgvRaw;

	for (size_t i = 0; i < commandArgv.size(); ++i) {
		commandArgvRaw.push_back(commandArgv[i].c_str());
	}
	commandArgvRaw.push_back(nullptr);

	if (commands[commandArgv[0]].registered) {
		uint8_t ret = commands[commandArgv[0]].function(commandArgv, commands);
		if (setenv("?", std::to_string(ret).c_str(), 1) == -1) {
			perror("[ERROR] setenv failed");
			return 1;
		}
		return 0;
	}

	pid_t pid = fork();
	if (pid == 0) { // child
		if (execvp(commandArgvRaw[0], (char**)commandArgvRaw.data()) == -1) {
			perror("[ERROR] Command execution failed");
		}
		exit(0);
	}
	else if (pid == -1) { // error
		perror("[ERROR] fork");
		return 1;
	}
	else { // parent
		int status;
		pid = wait(&status);
	}

	return 0;
}

uint8_t BuiltInCommands::Invert(std::vector <std::string> argv, CommandMap commands) {
	(void) argv;
	(void) commands;
	
	char* commandRetValue = getenv("?");
	if (commandRetValue == nullptr) {
		printf("[ERROR] Else: failed to get return value");
		return 1;
	}
	uint8_t commandRetBool = atoi(commandRetValue);

	if (commandRetBool == RETCODE_TRUE) {
		return RETCODE_FALSE;
	}
	return RETCODE_TRUE;
}