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
	if (!FS::File::Exists(std::string(getenv("HOME")) + "/.config/ysh2/init.ysh")) {
		FS::File::Create(std::string(getenv("HOME")) + "/.config/ysh2/init.ysh");
		FS::File::Write(std::string(getenv("HOME")) + "/.config/ysh2/init.ysh",
			"set YSH_PROMPT \"\\e[32m\\u:\\e[36m\\w\\e[0m> \"\n"
			"echo Welcome to $YSH_APP_NAME $YSH_APP_VERSION\n"
		);
	}

	// loop through arguments
	bool        runScript = false;
	std::string scriptPath;
	for (int i = 1; i < argc; ++i) {
		std::string currentArg = argv[i];
		if (currentArg[0] == '-') {
			if ((currentArg == "-h") || (currentArg == "--help")) {
				printf(
					"Help menu for %s\n"
					"  -v / --version : show app name and app version\n"
					"  -t / --tokens  : display lexer output after command inputs\n"
				, APP_NAME);
				exit(0);
			}
			if ((currentArg == "-v") || (currentArg == "--version")) {
				printf("%s %s\n", APP_NAME, APP_VERSION);
				exit(0);
			}
			if ((currentArg == "-t") || (currentArg == "--tokens")) {
				options.showTokens = true;
			}
		}
		else {
			// looks like the user wants us to run a script
			runScript  = true;
			scriptPath = currentArg;
		}
	}

	if (runScript) {
		ExecuteScript(FS::File::Read(scriptPath));
		exit(0);
	}

	// set variables
	run                 = true;
	oldWorkingDirectory = Util::GetWorkingDirectory();

	// set env variables
	if (getenv("YSH_PROMPT") == nullptr) {
		setenv("YSH_PROMPT", "$ ", 0);
	}
	if (getenv("YSH_ROOTPROMPT") == nullptr) {
		setenv("YSH_ROOTPROMPT", "# ", 0);
	}
	setenv("SHELL", argv[0], 1);
	setenv("YSH_APP_NAME", APP_NAME, 1);
	setenv("YSH_APP_VERSION", APP_VERSION, 1);

	// register commands
	RegisterCommand("help", BuiltInCommands::Help, {
		"help [command]",
		"  shows you how to use a command"
	});
	RegisterCommand("exit", BuiltInCommands::Exit, {
		"exit {status}",
		"  makes ysh quit"
		"  if {status} is given, exit with exit code {status}"
		"  {status} must be a number"
	});
	RegisterCommand("set", BuiltInCommands::Set, {
		"set [key] [value]",
		"  sets an enviroment variable of name [key] to [value]"
	});
	RegisterCommand("cd", BuiltInCommands::Cd, {
		"cd {directory}",
		"  changes working directory to {directory}, or if {directory} is not given change working directory to current user's home folder"
	});
	RegisterCommand("strcmp", BuiltInCommands::Strcmp, {
		"strcmp [string1] [string2]",
		"  returns 0 (true) if [string1] and [string2] are the same, else return 1 (false)",
		"  returns 255 on error"
	});
	RegisterCommand("if", BuiltInCommands::If, {
		"if [command] [args] ...",
		"  executes [command] with [args] if the enviroment variable ? is equal to 0"
	});
	RegisterCommand("invert", BuiltInCommands::Invert, {
		"invert",
		"  inverts the boolean in the enviroment variable ?",
		"  if ? is true then set it to false",
		"  if ? is false then set it to true"
	});

	// load and run yshrc
	std::vector <std::string> scriptLines = FS::File::ReadIntoVector(std::string(getenv("HOME")) + "/.config/ysh2/init.ysh");
	for (size_t i = 0; i < scriptLines.size(); ++i) {
		ExecuteScript(scriptLines[i]);
	}
}

void App::Update() {
	const char* promptRaw = getenv("YSH_PROMPT");
	if (promptRaw == nullptr) {
		promptRaw = "$ ";
	}

	rawInput = readline(Util::Escape(promptRaw).c_str());
	add_history(rawInput);
	if (rawInput == nullptr) {
		puts("readline returned NULL");
		while (true) {}
	}

	input = rawInput;
	free(rawInput);

	// handle tokens in user input
	input = Util::StringReplaceAll(input, "~", getenv("HOME"));

	ExecuteScript(input);
}

App::~App() {
	puts("exit");
}