#pragma once
#include <string>
#include <queue>
namespace pt {
enum class TYPETOKEN { TYPE, IDENTIFIER, LOOKAHEAD, VALUE, VALUE_STRING, ESCAPE, UNKNOWN };

class ParsConfig {

private:
	std::string m_input;
	struct Token {
		std::string lexem;
		TYPETOKEN type;
	};
	struct bracket {
		char value;
		int line;
	};
	int check_brackets(char* input);
	Token& get_token(std::queue<Token>& tokens);
	void connect_config();
	std::queue<Token>& lexer(char* input);
	
public:
	ParsConfig();
    
};

}