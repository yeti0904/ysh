#include "lexer.hh"

std::vector <Lexer::Token> Lexer::Lex(std::string src) {
	std::vector <Lexer::Token> ret;
	std::string                reading;
	bool                       command  = false;
	bool                       inString = false;

	for (size_t i = 0; i <= src.length(); ++i) {
		switch (src[i]) {
			case '"':  // string
			case '\0': // null terminator (end of string)
			case '\n': // new line
			case ' ': {
				if (src[i] == '"') {
					if (!inString) {
						inString = true;
						break;
					}
					inString = false;
				}
				if (inString && (src[i] == ' ')) {
					reading += src[i];
					break;
				}
				if (reading == "") {
					break;
				}
				if (command && (src[i] != '\n')) {
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
	}
	return "";
}