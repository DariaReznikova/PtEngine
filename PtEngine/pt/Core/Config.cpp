#include "Logger.hpp"
#include "Config.hpp"
#include <string>
#include <string>
#include <vector>
#include <stack>

namespace pt {

ConfigParser::ConfigParser(std::string pathToFile) : m_pathToFile { pathToFile }{ 
    FILE* file;
    if (fopen_s(&file, m_pathToFile.c_str(), "r") != 0) {
        PT_LOG_FATAL("Can't open configuration file '{}'", m_pathToFile);                     
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
                while ((*inputIterator) == ' ' || (*inputIterator) == '\t' || (*inputIterator) == '\r' || (*inputIterator) == '\v' || (*inputIterator) == '\f'
                    && inputIterator != (m_input.end() - 1)) {
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
                        m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE });
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
                    while (isdigit(*inputIterator) || (*inputIterator) == '.') {
                        temp += (*inputIterator);
                        ++inputIterator;
                    }
                    m_tokens.push_back({ tempToken.lexem += temp, tempToken.type = TokenType::VALUE });
					tempToken.lexem.clear();
                    temp.clear();
                }

                else if (strchr(":=,[]{}", (*inputIterator)) || (*inputIterator) == '\n' || (*inputIterator) == '\n\r') {
					if ((*inputIterator) == '\n' || (*inputIterator) == '\n\r'){
						++lineWithUnknownToken;
					}
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
}