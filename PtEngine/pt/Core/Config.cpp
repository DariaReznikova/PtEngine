#include "Logger.hpp"
#include "Config.hpp"
#include <string>
#include <iostream>
#include <string>
#include <queue>
#include <stack>

namespace pt {

ConfigParser::ConfigParser(std::string pathToFile) { 
    FILE* file;
    if (fopen_s(&file, pathToFile.c_str(), "r") != 0) {
        PT_LOG_FATAL("Can't open configuration file '{}'", pathToFile);                     
    }
    else {
        while (!feof(file)) {
            m_input += fgetc(file);
        }
    }
    fclose(file);
    if (!m_input.empty()) {
        lexer(const_cast<char*>(m_input.c_str()));
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

std::queue<ConfigParser::Token>& ConfigParser::lexer(char* input){

	std::queue<Token> tokens;
	Bracket optErrorInfo;
	bool bracketResult = m_checkBracket(optErrorInfo);
	if (!bracketResult){
		PT_LOG_WARNING("Incorrect symbol {} configuration file in line {}", optErrorInfo.value, optErrorInfo.line);
		while (!tokens.empty()) {
			tokens.pop();
		}
		return tokens;
	}
	else {
		std::string temp;
		int offset = 0;

		while ((*input) != '\0') {
			
			while ((*input) == ' ' && (*input) != '\0') {
				++input;
			}
			
			if (isalpha(*input)) {
				offset = 0;
				while (isalpha(*input) || isdigit(*input)) {
					temp += (*input);
					++input;
					++offset;
				}
				if (temp == "bool" || temp == "string" || temp == "table"
					|| temp == "i8" || temp == "i16" || temp == "i32" || temp == "i64"
					|| temp == "u8" || temp == "u16" || temp == "u32" || temp == "u64"
					|| temp == "f32" || temp == "f64") {
					tokens.push({ temp, TYPETOKEN::TYPE });
					temp.clear();
					continue;
				}
				else {
					input -= offset;
				}
				temp.clear();
			}

			if ((*input) == '"') {
				++input;
				while ((*input) != '"') {
					if ((*input) == '\n' || (*input) == '\n\r' || (*input) == '\'') {
						tokens.push({ std::to_string((*input)), TYPETOKEN::ESCAPE });
					}
					else {
						temp += (*input);
						++input;
					}
				}
				++input;
				tokens.push({ temp, TYPETOKEN::VALUE_STRING });
				temp.clear();
				continue;
			}

			if (isalpha(*input)) {
				while (isalpha(*input) || isdigit(*input) || (*input) == '_') {
					temp += (*input);
					++input;
				}
				if (temp == "false" || temp == "true") {
					tokens.push({ temp, TYPETOKEN::VALUE });
					temp.clear();
					continue;
				}
				else {
					tokens.push({ temp, TYPETOKEN::IDENTIFIER });
					temp.clear();
					continue;
				}
			}

			if (isdigit(*input) || (*input) == '-') {
				while (isdigit(*input) || (*input) == '.' ) {
					temp += (*input);
					++input;
				}
				tokens.push({ temp, TYPETOKEN::VALUE });
				temp.clear();
				continue;
			}

			if (strchr(":=,[]{}", (*input)) || (*input) == '\n' || (*input) == '\n\r') {
				tokens.push({ std::to_string((*input)), TYPETOKEN::LOOKAHEAD });
				++input;
				continue;
			}

			else {
				temp += (*input);
				tokens.push({ temp, TYPETOKEN::UNKNOWN });
				PT_LOG_WARNING(" Unknown character {} found in the configuration file", temp);
				temp.clear();
				++input;
				continue;
			}
		}
		return tokens;
	}
}
ConfigParser::Token& ConfigParser::get_token(std::queue<Token>& tokens) {
	static Token temp = tokens.back();
	tokens.pop();
	return temp;
}

}


