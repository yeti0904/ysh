#pragma once

// C standard libraries
#include <stdio.h>
#include <stdlib.h>

// C platform specific standard libraries
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef __linux__
	#include <linux/limits.h>
#else
	#include <limits.h>
#endif

// C++ standard libraries
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

// C libraries
#include <readline/readline.h>

// C++ libraries
#include "../luacpp/include/luacpp/luacpp" /* https://github.com/elelel/luacpp */