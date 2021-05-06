#pragma once
#include <string>
#include <vector>

namespace pt {

enum class TokenType { TYPE, IDENTIFIER, LOOKAHEAD, VALUE, VALUE_STRING, ESCAPE, UNKNOWN };

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
    std::string m_input;
    std::string m_pathToFile;
    std::vector<Token> m_tokens;
    bool m_checkBracket(Bracket &optErrorInfo);
    Token& get_token(std::vector<Token>& tokens);
    void m_parseLexemes();
};

}