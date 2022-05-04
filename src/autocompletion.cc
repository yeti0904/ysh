#include "autocompletion.hh"
#include "app.hh"

void App::GetExecutables() {
	// get directories in PATH
	pathDirectories = {};
	{
		char* pathRaw = getenv("PATH");
		if (pathRaw == nullptr) {
			fputs("[ERROR] failed to get PATH", stderr);
		}
		std::string path = pathRaw;
		std::string reading;
		for (size_t i = 0; i <= path.length(); ++i) {
			switch (path[i]) {
				case '\0':
				case ':': {
					pathDirectories.push_back(reading);
					reading = "";
					break;
				}
				default: {
					reading += path[i];
					break;
				}
			}
		}
	}

	/*for (size_t i = 0; i < pathDirectories.size(); ++i) {
		puts(pathDirectories[i].c_str());
	}
	puts("end");*/

	// search through all directories
	executables = {};
	for (size_t i = 0; i < pathDirectories.size(); ++i) {
		DIR*           dhnd = opendir(pathDirectories[i].c_str());
		struct dirent* dir;
		if (dhnd == nullptr) {
			continue;
		}
		while ((dir = readdir(dhnd)) != nullptr) {
			if (dir->d_type != DT_DIR) {
				executables.push_back(dir->d_name);
			}
		}
		if (closedir(dhnd) == -1) {
			perror("[ERROR] closedir failed");
			exit(1);
		}
	}
}

std::vector <std::string> globalExecutables;

void AutoCompletion::Init(std::vector <std::string> executables) {
	globalExecutables = executables;
}

char** AutoCompletion::Completion(const char* text, int, int) {
	return rl_completion_matches(text, AutoCompletion::Generator);
}

char* AutoCompletion::Generator(const char* text, int state) {
	//printf("size: %i\n", (int) globalExecutables.size());
	size_t length = strlen(text);

	for (size_t i = 0; i < globalExecutables.size(); ++i) {
		if (std::string(text) == globalExecutables[i].substr(0, length)) {
			-- state;
			if (state > 0) {
				continue;
			}
			//printf("returning %s\n", globalExecutables[i].c_str());
			return strdup(globalExecutables[i].c_str());
		}
	}

	//puts("returning nothing");
	return NULL; // found nothing
}