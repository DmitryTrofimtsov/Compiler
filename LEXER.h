#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

class TOKEN {
public:
	size_t m_line;
	size_t m_column;
	size_t m_id;
	std::string m_value;

	TOKEN() {};
	TOKEN(size_t line, size_t column, size_t id, std::string& value);
	TOKEN(const TOKEN& token);
};

class LEXER
{
public:
	LEXER();
	~LEXER();
	
	void Operation(const std::string& path);
	std::unique_ptr<std::list<TOKEN>>& GetTableTokens() { return m_table_of_tokens; }
	const std::unique_ptr<std::list<std::string>>& GetTableErrors() const { return m_table_of_errors; }

protected:
	void FillSymbolCategories();
	void FillMultisymbolDelimiters();
	void FillKeyWords();

	void Whitespace();
	void Constant();
	void Identifier();
	void SimpleDelimiter();
	void DoubleDelimiter();
	void Comment();
	void Error();

	void OutputInFile();

private:
	std::unique_ptr<std::ifstream> m_input_file;
	std::unique_ptr<std::ofstream> m_generated_file;

	std::unique_ptr<std::vector<size_t>> m_symbol_categories;
	std::unique_ptr<std::unordered_map<std::string, size_t>> m_multisymbol_delimiters;
	std::unique_ptr<std::unordered_map<std::string, size_t>> m_key_words;
	std::unique_ptr<std::unordered_map<std::string, size_t>> m_constans;
	std::unique_ptr<std::unordered_map<std::string, size_t>> m_identifiers;

	std::unique_ptr<std::list<TOKEN>> m_table_of_tokens;
	std::unique_ptr<std::list<std::string>> m_table_of_errors;

	size_t m_line = 1;
	size_t m_column = 1;

	char m_char;
};

