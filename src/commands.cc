#include "commands.hh"
#include "fs.hh"
#include "constants.hh"
#include "app.hh"

uint8_t BuiltInCommands::Help(std::vector <std::string> argv, CommandMap commands, App) {
	if (argv.size() == 1) {
		/*puts("[ERROR] Help: not enough arguments");
		return 1;*/
		printf("Welcome to %s\nCommands: ", APP_NAME);
		for (const auto& [key, val] : commands) {
			printf("%s, ", key.c_str());
		}
		puts("\b\b  \b\b"); // this looks ugly i know
		return 0;
	}
	if (!commands[argv[1]].registered) {
		fputs("[ERROR] Help: command doesnt exist\n", stderr);
		return 1;
	}
	for (size_t i = 0; i < commands[argv[1]].helpItems.size(); ++i) {
		puts(commands[argv[1]].helpItems[i].c_str());
	}
	return 0;
}

uint8_t BuiltInCommands::Exit(std::vector <std::string> argv, CommandMap, App) {

	uint8_t exitCode = 0;

	// get exit code given by user if there is one
	if (argv.size() > 1) {
		exitCode = std::stoi(argv[1]);
	}

	exit(exitCode);
	return exitCode;
}

uint8_t BuiltInCommands::Set(std::vector <std::string> args, CommandMap commands, App app) {
	(void) commands;
	(void) app;
	if (args.size() < 3) {
		fputs("[ERROR] Set: not enough arguments\n", stderr);
		return 1;
	}
	setenv(args[1].c_str(), args[2].c_str(), 1);
	return 0;
}

uint8_t BuiltInCommands::Cd(std::vector <std::string> argv, CommandMap commands, App app) {
	(void) commands;
	(void) app;

	if (argv.size() < 2) {
		chdir(getenv("HOME"));
		return 0;
	}

	if (FS::Directory::Exists(argv[1])) {
		chdir(argv[1].c_str());
	}
	else {
		fputs("[ERROR] Cd: Directory doesn't exist\n", stderr);
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

uint8_t BuiltInCommands::If(std::vector <std::string> argv, CommandMap commands, App app) {
	if (argv.size() < 2) {
		fputs("[ERROR] If: not enough arguments\n", stderr);
	}

	char* commandRetValue = getenv("?");
	if (commandRetValue == nullptr) {
		fputs("[ERROR] If: failed to get return value\n", stderr);
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
		uint8_t ret = commands[commandArgv[0]].function(commandArgv, commands, app);
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

uint8_t BuiltInCommands::Invert(std::vector <std::string> argv, CommandMap commands, App app) {
	(void) argv;
	(void) commands;
	(void) app;
	
	char* commandRetValue = getenv("?");
	if (commandRetValue == nullptr) {
		fputs("[ERROR] Else: failed to get return value\n", stderr);
		return 1;
	}
	uint8_t commandRetBool = atoi(commandRetValue);

	if (commandRetBool == RETCODE_TRUE) {
		return RETCODE_FALSE;
	}
	return RETCODE_TRUE;
}

uint8_t BuiltInCommands::ListExecutables(std::vector <std::string> argv, CommandMap commands, App app) {
	(void) argv;
	(void) commands;
	for (size_t i = 0; i < app.executables.size(); ++i) {
		puts(app.executables[i].c_str());
	}
	return 0;
}