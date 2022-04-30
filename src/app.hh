#pragma once
#include "_components.hh"
#include "commands.hh"

struct AppOptions {
	bool showTokens;
};

class App {
	public:
		// variables
		bool        run;
		char*       rawInput;
		std::string input;
		CommandMap  commands;
		AppOptions  options;
		std::string oldWorkingDirectory;

		std::vector <std::string> commandArgv;
		std::vector <const char*> commandArgvRaw;

		// app functions
		App(int argc, char** argv);
		void Update();
		void Shell();
		~App();

		// util functions
		void RegisterCommand(std::string name, CommandFunction function, std::vector <std::string> helpItems);
		void ExecuteScript(std::string input);
};