#pragma once
#include "Tree.hpp"
#include <string>
#include <vector>

namespace pt {

const short MINI8 = -128;
const short MAXI8 = 127;
const int16_t MINI16 = -32768;
const int16_t MAXI16 = 32767;
const int32_t MINI32 = -2147483648;
const int32_t MAXI32 = 2147483647;
const int64_t MINI64 = -9223372036854775807;
const int64_t MAXI64 = 9223372036854775807;
const int MINU = 0;
const short MAXU8 = 256;
const uint16_t MAXU16 = 65535;
const uint32_t MAXU32 = 4294967295;
const uint64_t MAXU64 = 18446744073709551615;

enum class TokenType { TYPE, IDENTIFIER, LOOKAHEAD, VALUE_INTEGER, VALUE_FLOAT, VALUE_STRING, VALUE_BOOL, ESCAPE, UNKNOWN, END };

class ConfigParser : public Tree {
public:
    ConfigParser(std::string pathToFile);
    Tree m_ast;
private:
    struct Token {
        std::string lexem;
        TokenType type;
    };
    struct Bracket {
        char value;
        int line;
    };
    struct tableItemFull {
        bool isHasIndifier = false;
        bool isHasType = false;
        bool isHasValue = false;
        bool isHasColon = false;
        bool isHasEqual = false;
    };
    std::string m_pathToFile;
    std::string m_input; 
    std::vector<Token> m_tokens;
    bool m_isTokensEnded = false;

    bool m_checkBracket(Bracket &optErrorInfo);
    Token m_getToken(int offset = 0);
    void m_parseLexemes();

    Tree m_table();
    void m_tableBody(Token token, Tree& ast);
    void m_tableItem(Token token, Tree& ast, tableItemFull& checkTableItem);
    void m_terminate(Token token, Tree& ast);
};

}