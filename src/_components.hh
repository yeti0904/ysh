#pragma once

// C standard libraries
#include <stdio.h>
#include <stdlib.h>

// C platform specific standard libraries
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
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
#include <readline/history.h>