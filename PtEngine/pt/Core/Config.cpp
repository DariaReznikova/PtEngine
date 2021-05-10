#include "Config.hpp"
#include "Logger.hpp"
#include "Tree.hpp"
#include <string>
#include <vector>
#include <stack>

namespace pt {

ConfigParser::ConfigParser(std::string pathToFile) : m_pathToFile { pathToFile }{ 
    FILE* file;
    if (fopen_s(&file, m_pathToFile.c_str(), "r") != 0) {
        PT_LOG_FATAL("Can't open in configuration file '{}'", m_pathToFile);                     
    }
    else {
        while (!feof(file)) {
            m_input += fgetc(file);
        }
    }
    fclose(file);
    if (!m_input.empty()) {
        m_parseLexemes();
        m_tokens;
        m_ast = m_table();
    }
    else{
        PT_LOG_ERROR("Configuration file '{}' is empty", m_pathToFile);
    }
}
bool ConfigParser::m_checkBracket(Bracket &optErrorInfo) {
    int lineWithError = 1;
    size_t offset = 0;
    int lineBracketsCount = 0;
    bool isNewLine = true;

    std::stack<Bracket> stack;
    Bracket temp;
    stack.push({ temp.value = 'v', temp.line = -1 }); // first accessing in expression [0]
    auto inputIterator = m_input.begin();
    while (inputIterator != (m_input.end() - 1)) {
        if (isNewLine) {
            lineBracketsCount = 0;
            int lineOffset = 0;
            auto checkEscIterator = (m_input.begin() + 1);
            while ((*inputIterator) != '\n' && (*inputIterator) != '\n\r' && inputIterator != (m_input.end() - 1)) { // check count ["] in line 
                if ((*inputIterator) == '"' && (*checkEscIterator) != '\\') {
                    ++lineBracketsCount;
                }
                ++inputIterator;
                ++checkEscIterator;
                ++lineOffset;
            }
            inputIterator -= lineOffset;
            checkEscIterator -= lineOffset;
            isNewLine = false;

            if (lineBracketsCount & 1) {
                optErrorInfo.line = lineWithError;
                optErrorInfo.value = '"';
                return false;
            }
        }
        if ((*inputIterator) == '\n' || (*inputIterator) == '\n\r') {
            ++lineWithError;
            isNewLine = true;
        }

        if ((*inputIterator) == '{' || (*inputIterator) == '[') {
            stack.push({ temp.value = (*inputIterator), temp.line = lineWithError });
        }
        else if ( ((*inputIterator) == '}' && stack.top().value == '{') // [0]
            ||((*inputIterator) == ']' && stack.top().value == '[')) {
            stack.pop();
        }
        else if ((*inputIterator) == '{' || (*inputIterator) == '[' 
            ||(*inputIterator) == '}' || (*inputIterator) == ']' ) {
            optErrorInfo.line = lineWithError;
            optErrorInfo.value = (*inputIterator);
            return false;
        }
        ++inputIterator;
        ++offset;
    }
    if (stack.size() == 1) {
        optErrorInfo.line = -1;
        optErrorInfo.value = 'v';
        return true;
    }
    else {
        optErrorInfo.line = stack.top().line;
        optErrorInfo.value = stack.top().value;
        return false;
    }
}

void ConfigParser::m_parseLexemes() {
    Bracket optErrorInfo;
    bool bracketResult = m_checkBracket(optErrorInfo);
        if (!bracketResult) {
            PT_LOG_ERROR("Incorrect symbol configuration file '{}' in line '{}'", m_pathToFile, optErrorInfo.line);
            m_tokens.clear();
        }
        else {
            int lineWithUnknownToken = 1;
            std::string temp;
            int offset = 0;
            Token tempToken;
            auto inputIterator = m_input.begin();
            while (inputIterator != (m_input.end() - 1)) {
                if ((*inputIterator) == '\n' || (*inputIterator) == '\n\r'){
                        ++lineWithUnknownToken;
                    }
                while (isspace(*inputIterator) && inputIterator != (m_input.end() - 1)) {
                    ++inputIterator;
                }
                if (isalpha(*inputIterator)) {
                    offset = 0;
                    while (isalpha(*inputIterator) || isdigit(*inputIterator)) {
                        temp += (*inputIterator);
                        ++inputIterator;
                        ++offset;
                    }
                    if (temp == "bool" || temp == "string" || temp == "table"
                        || temp == "i8" || temp == "i16" || temp == "i32" || temp == "i64"
                        || temp == "u8" || temp == "u16" || temp == "u32" || temp == "u64"
                        || temp == "f32" || temp == "f64") {
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::TYPE });
                        tempToken.lexem.clear();
                        temp.clear();
                        continue;
                    }
                    else {
                        inputIterator -= offset;
                    }
                    temp.clear();
                }
                else if ((*inputIterator) == '"') {
                    ++inputIterator;
                    while ((*inputIterator) != '"') {
                        if ((*inputIterator) == '\n' || (*inputIterator) == '\n\r' || (*inputIterator) == '\'') {
                            m_tokens.push_back({ tempToken.lexem += (*inputIterator), tempToken.type = TokenType::ESCAPE });
                            tempToken.lexem.clear();
                        }
                        else {
                            temp += (*inputIterator);
                            ++inputIterator;
                        }
                    }
                    ++inputIterator;
                    m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE_STRING });
                    tempToken.lexem.clear();
                    temp.clear();
                }

                if (isalpha(*inputIterator)) {
                    while (isalpha(*inputIterator) || isdigit(*inputIterator) || (*inputIterator) == '_') {
                        temp += (*inputIterator);
                        ++inputIterator;
                    }
                    if (temp == "false" || temp == "true") {
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE_BOOL });
                        tempToken.lexem.clear();
                        temp.clear();
                        continue;
                    }
                    else {
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::IDENTIFIER });
                        tempToken.lexem.clear();
                        temp.clear();
                        continue;
                    }
                }

                else if (isdigit(*inputIterator) || (*inputIterator) == '-') {
                    if ((*inputIterator) == '-') {
                        temp += (*inputIterator);
                        ++inputIterator;
                    }
                    bool isFloat = false;
                    while (isdigit(*inputIterator) || (*inputIterator) == '.') {
                        if ((*inputIterator) == '.') {
                            isFloat = true;
                        }
                        temp += (*inputIterator);
                        ++inputIterator;
                    }
                    if (isFloat) {
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE_FLOAT });
                        tempToken.lexem.clear();
                    }
                    else {
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE_INTEGER });
                        tempToken.lexem.clear();
                    }
                    temp.clear();
                }

                else if (strchr(":=,[]{}", (*inputIterator))) {
                    m_tokens.push_back({ tempToken.lexem += (*inputIterator), tempToken.type = TokenType::LOOKAHEAD });
                    tempToken.lexem.clear();
                    ++inputIterator;
                }

                else {
                    temp += (*inputIterator);
                    m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::UNKNOWN });
                    PT_LOG_ERROR("Unknown character '{}' found in the configuration file '{}' in line '{}'", temp, m_pathToFile, lineWithUnknownToken);
                    tempToken.lexem.clear();
                    m_tokens.clear();
                    return;
                }
            }
        }
    }

ConfigParser::Token ConfigParser::m_getToken(int offset) {
        if (!m_tokens.empty()) {
            static int s_currentIndex = 0;
            if (offset) {
                offset += 1;
                Token temp = m_tokens.at(s_currentIndex - offset);
                return temp;
            }     
            if (s_currentIndex >= m_tokens.size()) {
                m_isTokensEnded = true;
                Token temp;
                temp.type = TokenType::END;
                temp.lexem = "";
                return temp;
            }      
            else {
                Token temp = m_tokens.at(s_currentIndex);
                ++s_currentIndex;
                return temp;
            }       
        }
        else {
            PT_LOG_ERROR("Configuration file '{}' is empty", m_pathToFile);
        }
    }

    Tree ConfigParser::m_table() { 
        Tree ast;
        ast.m_addNode(NodesType::TABLE);
        Token token = m_getToken();
        while(!m_isTokensEnded) {
            ast.m_addNode(NodesType::TABLEBODY);
            m_tableBody(token, ast);
            token = m_getToken(); 
         } 
        return ast;
    }
    
    void ConfigParser::m_tableBody(Token token, Tree& ast) { 
        Token next = token;
        bool enter = false; // enter from recoursive
        while(!m_isTokensEnded){
        switch (next.type) {
        case TokenType::LOOKAHEAD:
            if (next.lexem == "{") {
                next = m_getToken();
                ast.m_addNode(NodesType::TABLEBODY);
                m_tableBody(next, ast);
                if (enter) { return; }
            }
            else if (next.lexem == "}") {
                next = m_getToken();
                if(next.lexem == ","){
                    ast.m_ascent();
                    m_tableBody(next, ast);
                    if (enter) { return; }
                }
            }
            else if(next.lexem == ","){
                next = m_getToken();
                m_tableBody(next, ast);
                if (enter) { return; } 
            }
            /*else {
                PT_LOG_FATAL("Can't parsing configuration file '{}' - syntactic error", m_pathToFile);
            }*/
        }
        if (!m_isTokensEnded) {
            tableItemFull checkTableItem;
            ast.m_addNode(NodesType::TABLEITEM);
            m_tableItem(token, ast, checkTableItem);
            while (!checkTableItem.isHasIndifier || !checkTableItem.isHasType || !checkTableItem.isHasValue || !checkTableItem.isHasColon || !checkTableItem.isHasEqual) {
                Token temp = m_getToken();
                m_tableItem(temp, ast, checkTableItem);
            }
            ast.m_ascent();
            ast.m_ascent();
            next = m_getToken();
            m_tableBody(next, ast);
            if (enter) { return; }
        }
        }
        enter = true;
        return;
    }

    void ConfigParser::m_tableItem(Token token, Tree& ast, tableItemFull& checkTableItem) {
        switch (token.type) {
        case TokenType::IDENTIFIER:  
            ast.m_addNode(NodesType::IDENTIFIER, token.lexem, false);
            checkTableItem.isHasIndifier = true;
            break;
        case TokenType::LOOKAHEAD:
            if (token.lexem == ":" && m_getToken(1).type == TokenType::IDENTIFIER) {
                checkTableItem.isHasColon = true;
            }
            else if (token.lexem == "=" && m_getToken(1).type == TokenType::TYPE) {
                checkTableItem.isHasEqual = true;
            }
            else {
                PT_LOG_FATAL("Can't parsing configuration file '{}' - syntactic error", m_pathToFile);
            }
            break;
        case TokenType::TYPE:
            if (token.lexem == "table") {
            ast.m_addNode(NodesType::TYPE, token.lexem, true);
            checkTableItem.isHasType = true;
            checkTableItem.isHasValue = true;
            Token next = m_getToken();
            if (next.lexem == "=" && m_getToken(1).type == TokenType::TYPE) {
                checkTableItem.isHasEqual = true;
                next = m_getToken();
            }
            else {
                PT_LOG_FATAL("Can't parsing configuration file '{}' - syntactic error", m_pathToFile);
            }
            m_tableBody(next, ast);
            break;
            }
            else {
                ast.m_addNode(NodesType::TYPE, token.lexem, true);
                checkTableItem.isHasType = true;
                break;
            }
        case TokenType::VALUE_BOOL:
        case TokenType::VALUE_STRING:
        case TokenType::VALUE_INTEGER:
        case TokenType::VALUE_FLOAT:
            m_terminate(token, ast);
            checkTableItem.isHasValue = true;
            break;
        default:
            PT_LOG_FATAL("Can't parsing configuration file '{}' - syntactic error", m_pathToFile);
        }
    }
    void ConfigParser::m_terminate(Token token, Tree& ast) {
        Token type = m_getToken(2);
        if (type.type == TokenType::TYPE) {
            if (type.lexem == "bool" && token.type == TokenType::VALUE_BOOL) {
                ast.m_addNode(NodesType::VALUE, token.lexem, true);
            }
            else if (type.lexem == "string" && token.type == TokenType::VALUE_STRING) {
                ast.m_addNode(NodesType::VALUE, token.lexem, true);
            }

            else if (type.lexem == "i8" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoi(token.lexem.c_str());
                if (value < MINI8 || value > MAXI8) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "i16" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoi(token.lexem.c_str());
                if (value < MINI16 || value > MAXI16) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "i32" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atol(token.lexem.c_str());
                if (value < MINI32 || value > MAXI32) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "i64" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoll(token.lexem.c_str());
                if (value < MINI64 || value > MAXI64) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "u8" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoi(token.lexem.c_str());
                if (value < MINU || value > MAXU8) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "u16" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoi(token.lexem.c_str());
                if (value < MINU || value > MAXU16) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "u32" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atol(token.lexem.c_str());
                if (value < MINU || value > MAXU32) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "u64" && token.type == TokenType::VALUE_INTEGER) {
                long long value = atoll(token.lexem.c_str());
                if (value < MINU || value > MAXU64) {
                    PT_LOG_ERROR("Entity type '{}' can't contain value '{}'", type.lexem, value);
                }
                else {
                    ast.m_addNode(NodesType::VALUE, token.lexem, true);
                }
            }
            else if (type.lexem == "f32" && token.type == TokenType::VALUE_FLOAT || token.type == TokenType::VALUE_INTEGER) {
                float f32 = atof(token.lexem.c_str());
                ast.m_addNode(NodesType::VALUE, token.lexem, true);

            }
            else if (type.lexem == "f64" && token.type == TokenType::VALUE_FLOAT || token.type == TokenType::VALUE_INTEGER) {
                double f64 = atof(token.lexem.c_str());
                ast.m_addNode(NodesType::VALUE, token.lexem, true);
            }
            else {
                PT_LOG_FATAL("Can't parsing configuration file '{}'", m_pathToFile);
            }
        }
    }

}
