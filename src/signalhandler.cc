#include "signalhandler.hh"

void SignalHandler::Init() {
	if (signal(SIGINT, SignalHandler::ControlC) == SIG_ERR) {
		perror("[ERROR] signal failed");
		exit(1);
	}
}

void SignalHandler::ControlC(int) {
	putchar('\n');// Move to a new line
	rl_on_new_line(); // Regenerate the prompt on a newline
	rl_replace_line("", 0); // Clear the previous text
	rl_redisplay();
} // https://stackoverflow.com/a/41917863