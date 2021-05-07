#pragma once
#include <string>
#include <vector>

namespace pt {

enum class TokenType { TYPE, IDENTIFIER, LOOKAHEAD, VALUE_INTEGER, VALUE_FLOAT, VALUE_STRING, VALUE_BOOL, ESCAPE, UNKNOWN, END };

class ConfigParser {
public:
    ConfigParser(std::string pathToFile);
private:
    struct Token {
        std::string lexem;
        TokenType type;
    };
    struct Bracket {
        char value;
        int line;
    };
    std::string m_pathToFile;
    std::string m_input; 
    std::vector<Token> m_tokens;
    bool m_isTokensEnded = false;

    bool m_checkBracket(Bracket &optErrorInfo);
    Token m_getToken(int offset);
    void m_parseLexemes();
};

}