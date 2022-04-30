#include "util.hh"

std::string Util::Escape(std::string src) {
	std::string ret;
	for (size_t i = 0; i < src.length(); ++i) {
		switch (src[i]) {
			case '\\': {
				if (i == src.length() - 1) break;
				++ i;
				switch (src[i]) {
					case 'e': {
						ret += '\033';
						break;
					}
					case 'n': {
						ret += '\n';
						break;
					}
					case 'r': {
						ret += '\r';
						break;
					}
					case 'u': {
						ret += getenv("USER");
						break;
					}
					case 'w': {
						ret += GetWorkingDirectory();
						break;
					}
					case '\\': {
						ret += '\\';
						break;
					}
					case '"': {
						ret += '"';
						break;
					}
					case '\'': {
						ret += '\'';
						break;
					}
				}
				break;
			}
			default: {
				ret += src[i];
			}
		}
	}

	return ret;
}

std::string Util::GetWorkingDirectory() {
	char* wd = (char*) malloc(PATH_MAX + 1);
	if (getcwd(wd, PATH_MAX + 1) == nullptr) {
		perror("[ERROR] getwd");
		exit(1);
	}

	std::string ret = wd;
	if (getenv("HOME") != nullptr) {
		ret = StringReplaceOne(ret, getenv("HOME"), "~");
	}
	free(wd);
	return ret;
}

std::string Util::StringReplaceAll(std::string src, std::string replaceWhat, std::string replaceWith) {
	std::string ret = src;
	while (ret.find(replaceWhat) != std::string::npos) {
		ret.replace(ret.find(replaceWhat), replaceWhat.length(), replaceWith);
	}
	return ret;
}

std::string Util::StringReplaceOne(std::string src, std::string replaceWhat, std::string replaceWith) {
	std::string ret = src;
	if (ret.find(replaceWhat) != std::string::npos) {
		ret.replace(ret.find(replaceWhat), replaceWhat.length(), replaceWith);
	}
	return ret;
}