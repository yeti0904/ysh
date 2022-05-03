#include "lexer.hh"
#include "util.hh"

std::vector <Lexer::Token> Lexer::Lex(std::string src) {
	std::vector <Lexer::Token> ret;
	std::string                reading;
	bool                       command  = false;
	bool                       inString = false;

	for (size_t i = 0; i <= src.length(); ++i) {
		switch (src[i]) {
			case ';':  // end of arguments
			case '"':  // string
			case '\'': // also string
			case '\0': // null terminator (end of string)
			case '\n': // new line
			case ' ': {
				if ((src[i] == '"') || (src[i] == '\'')) {
					if (!inString) {
						inString = true;
						break;
					}
					inString = false;
				}
				if (inString) {
					reading += src[i];
					break;
				}
				if (reading == "") {
					break;
				}
				if (command) {
					// found argument
					ret.push_back({
						Lexer::TokenType::Argument,
						reading
					});
				}
				else {
					// found command
					ret.push_back({
						Lexer::TokenType::Command,
						reading
					});
					command = true; // found the command, set this to true because now the arguments are coming
				}
				reading = "";

				if ((src[i] == '\n') || (src[i] == ';') || (src[i] == '\0')) {
					// end of arguments
					ret.push_back({
						Lexer::TokenType::EndOfArguments,
						""
					});
					command = false; // next command coming up (maybe)
				}

				break;
			}
			case '>': { // redirecting output
				if (inString) {
					reading += src[i];
					break;
				}
				if (reading.length() != 0) {
					if (!Util::StringIsNumerical(reading)) {
						printf("[ERROR] '%s' is not an integer\n", reading.c_str());
						exit(1);
					}
				}
				ret.push_back({
					Lexer::TokenType::RedirectOutput,
					reading
				});
				reading = "";
				break;
			}
			default: {
				reading += src[i];
				break;
			}
		}
	}

	return ret;
}

std::string Lexer::TokenToString(Lexer::Token token) {
	switch (token.type) {
		case Lexer::TokenType::Command: {
			return "Command";
		}
		case Lexer::TokenType::Argument: {
			return "Argument";
		}
		case Lexer::TokenType::EndOfArguments: {
			return "EndOfArguments";
		}
		case Lexer::TokenType::RedirectOutput: {
			return "RedirectOutput";
		}
	}
	return "";
}