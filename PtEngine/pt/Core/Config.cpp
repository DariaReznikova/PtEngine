#include "Logger.hpp"
#include "Config.hpp"
#include "fmt/format.h"
#include <iostream>
#include <string>
#include <queue>
#include <stack>

namespace pt {

ParsConfig::ParsConfig() { // if configuration file one
	 connect_config();
	if (!m_input.empty()) {
		lexer(const_cast<char*>(m_input.c_str()));
	}
}

int ParsConfig::check_brackets(char* input) {
	
	int line_error = 1;
	char offset = 0;
	int countOfStringBracket = 0;
	bool new_line = true;

	std::stack<bracket> stack;
	stack.push({ 'v', -1 });                              // first accessing in expression [0]

	while ((*input) != '\0'){

		if (new_line) {
			countOfStringBracket = 0;
			int offset_line = 0;
			while ((*input) != '\n' && (*input) != '\n\r' && (*input) != '\0') {       // check count ["] in line
				if ((*input) == '"') {
					++countOfStringBracket;
				}
				++input;
				++offset_line;
			}
			input -= offset_line;
			new_line = false;

			if ((countOfStringBracket & 1)) {
				input -= offset;
				return line_error;
			}
		}
		if ((*input) == '\n' || (*input) == '\n\r') {
			++line_error;
			new_line = true;
		}

		if ((*input) == '{' || (*input) == '[') {
			stack.push({ (*input), line_error });
		}
		else if ( ((*input) == '}' && stack.top().value == '{') // [0]
			||((*input) == ']' && stack.top().value == '[')) {
			stack.pop();
		}
		else if ((*input) == '{' || (*input) == '[' 
			||(*input) == '}' || (*input) == ']' ) {
			input -= offset;
			return line_error;
		}
		++input;
		++offset;
	}

	if (stack.size() == 1) {
		input -= offset;
		return 0;
	}
	else {
		input -= offset;
		return stack.top().line;
	}
}

std::queue<ParsConfig::Token>& ParsConfig::lexer(char* input){

	std::queue<ParsConfig::Token> tokens;
	if (check_brackets(input) == 0) {
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
	else {
		int error_place = check_brackets(input);
		PT_LOG_WARNING(" Incorrect bracket configuration file in line {}", error_place);
		std::cerr << error_place;
		while (!tokens.empty()) {
			tokens.pop();
		}
		return tokens;
	}
}

ParsConfig::Token& ParsConfig::get_token(std::queue<ParsConfig::Token>& tokens) {
	static Token temp = tokens.back();
	tokens.pop();
	return temp;
}

void ParsConfig::connect_config() {
	
	char pathToFile[] = "config.txt";  // depends on the name of the file - need add to search log file in filesystem by extension
	FILE* file;
	if (fopen_s(&file, pathToFile, "r") != 0) {
		PT_LOG_ERROR(" Can't open configuration file");                     
		m_input.clear();
        exit(0);
		
	}
	else {
		while (!feof(file)) {
			m_input += fgetc(file);
		}
	}
	fclose(file);	
}
}
