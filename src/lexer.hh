#pragma once
#include "_components.hh"

namespace Lexer {
	enum class TokenType {
		Command = 0,
		Argument,
		EndOfArguments,
		RedirectOutput
	};
	struct Token {
		TokenType type;
		std::string content;
	};
	std::vector <Token> Lex(std::string src);
	std::string         TokenToString(Token token);
}