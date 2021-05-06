#pragma once
#include <string>
#include <queue>

namespace pt {

enum class TYPETOKEN { TYPE, IDENTIFIER, LOOKAHEAD, VALUE, VALUE_STRING, ESCAPE, UNKNOWN };

class ParsConfig {
public:
	ParsConfig();
private:
	struct Token {
		std::string lexem;
		TYPETOKEN type;
	};
	struct Bracket {
		char value;
		int line;
	};
	std::string m_input;

	bool m_checkBracket(Bracket &optErrorInfo);
	Token& get_token(std::queue<Token>& tokens);
	void connect_config();
	std::queue<Token>& lexer(char* input);

};
}