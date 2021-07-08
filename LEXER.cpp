#include "LEXER.h"

LEXER::LEXER() {
	FillSymbolCategories();
	FillKeyWords();
	FillMultisymbolDelimiters();

	m_constans = std::make_unique<std::unordered_map<std::string, size_t>>();
	m_identifiers = std::make_unique<std::unordered_map<std::string, size_t>>();

	m_table_of_tokens = std::make_unique<std::list<TOKEN>>();
	m_table_of_errors = std::make_unique<std::list<std::string>>();

	m_input_file = std::make_unique<std::ifstream>();
	m_generated_file = std::make_unique<std::ofstream>();
}

LEXER::~LEXER()
{
	if (m_input_file->is_open()) {
		m_input_file->close();
	}

	if (m_generated_file->is_open()) {
		m_input_file->close();
	}
}

void LEXER::Operation(const std::string& path)
{
	m_input_file->open(path + "/input.sig");
	
	if (!m_input_file->is_open()) {
		throw std::string("Failed to open file");
	}
	m_char = m_input_file->get();

	while (m_char != -1) {

		switch (m_symbol_categories->at(m_char)) {
		case 0:
			Whitespace();
			m_char = m_input_file->get();
			break;

		case 1:
			Constant();
			break;

		case 2:
			Identifier();
			break;

		case 3:
			SimpleDelimiter();
			m_column++;
			m_char = m_input_file->get();
			break;

		case 41:
			DoubleDelimiter();
			break;

		case 42:
			DoubleDelimiter();
			break;

		case 43:
			DoubleDelimiter();
			break;

		case 5:
			Comment();
			m_column++;
			m_char = m_input_file->get();
			break;

		default:
			Error();
			m_column++;
			m_char = m_input_file->get();
			break;
		}
	}
}

void LEXER::FillSymbolCategories() {
	m_symbol_categories = std::make_unique<std::vector<size_t>>(256, 6);

	for (size_t i = 8; i < 14; ++i) {
		m_symbol_categories->at(i) = 0;
	}

	for (size_t i = 48; i < 58; ++i) {
		m_symbol_categories->at(i) = 1;
	}

	for (size_t i = 65; i < 91; ++i) {
		m_symbol_categories->at(i) = 2;
	}

	m_symbol_categories->at(' ') = 0;
	m_symbol_categories->at('(') = 5;
	m_symbol_categories->at(':') = 41;
	m_symbol_categories->at(';') = 3;
	m_symbol_categories->at('<') = 42;
	m_symbol_categories->at('=') = 3;
	m_symbol_categories->at('>') = 43;
	m_symbol_categories->at('[') = 3;
	m_symbol_categories->at(']') = 3;
	m_symbol_categories->at('.') = 3;
}

void LEXER::FillMultisymbolDelimiters() {
	m_multisymbol_delimiters = std::make_unique<std::unordered_map<std::string, size_t>>();

	m_multisymbol_delimiters->insert({ ":=", 301 });
	m_multisymbol_delimiters->insert({ "<=", 302 });
	m_multisymbol_delimiters->insert({ ">=", 303 });
	m_multisymbol_delimiters->insert({ "<>", 304 });
}

void LEXER::FillKeyWords() {
	m_key_words = std::make_unique<std::unordered_map<std::string, size_t>>();

	m_key_words->insert({ "PROGRAM", 401 });
	m_key_words->insert({ "VAR", 402 });
	m_key_words->insert({ "BEGIN", 403 });
	m_key_words->insert({ "END", 404 });
	m_key_words->insert({ "INTEGER", 405 });
	m_key_words->insert({ "OR", 406 });
	m_key_words->insert({ "AND", 407 });
	m_key_words->insert({ "NOT", 408 });
}

void LEXER::Whitespace() {
	if (m_char == 10) {
		m_line++;
		m_column = 1;
	}
	else {
		m_column++;
	}
}

void LEXER::Constant() {
	TOKEN m_token;
	m_token.m_line = m_line;
	m_token.m_column = m_column;
	m_token.m_value.push_back(m_char);

	m_column++;

	while ((m_char = m_input_file->get()) != -1 && m_symbol_categories->at(m_char) == 1) {
		m_token.m_value.push_back(m_char);
		m_column++;
	}

	if (m_constans->find(m_token.m_value) == m_constans->end()) {
		m_constans->insert({ m_token.m_value, m_constans->size() + 501 });
	}

	m_token.m_id = m_constans->find(m_token.m_value)->second;
	m_table_of_tokens->push_back(m_token);
}

void LEXER::Identifier() {
	TOKEN m_token;
	m_token.m_line = m_line;
	m_token.m_column = m_column;
	m_token.m_value.push_back(m_char);

	m_column++;

	while ((m_char = m_input_file->get()) != -1 && (m_symbol_categories->at(m_char) == 1 || m_symbol_categories->at(m_char) == 2)) {
		m_token.m_value.push_back(m_char);
		m_column++;
	}

	if (m_key_words->find(m_token.m_value) != m_key_words->end()) {
		m_token.m_id = m_key_words->find(m_token.m_value)->second;
	}
	else if (m_identifiers->find(m_token.m_value) == m_identifiers->end()) {
		m_identifiers->insert({ m_token.m_value, m_identifiers->size() + 1001 });
		m_token.m_id = m_identifiers->find(m_token.m_value)->second;
	}
	else {
		m_token.m_id = m_identifiers->find(m_token.m_value)->second;
	}
	
	m_table_of_tokens->push_back(m_token);
}

void LEXER::SimpleDelimiter() {
	TOKEN m_token;
	m_token.m_line = m_line;
	m_token.m_column = m_column;
	m_token.m_value.push_back(m_char);
	m_token.m_id = m_char;
	m_table_of_tokens->push_back(m_token);
}

void LEXER::DoubleDelimiter() {
	TOKEN m_token;
	m_token.m_line = m_line;
	m_token.m_column = m_column;
	m_token.m_value.push_back(m_char);

	m_column++;
	m_char = m_input_file->get();
	m_token.m_value.push_back(m_char);

	if (m_multisymbol_delimiters->find(m_token.m_value) != m_multisymbol_delimiters->end()) {
		m_token.m_id = m_multisymbol_delimiters->find(m_token.m_value)->second;
		m_table_of_tokens->push_back(m_token);
		m_column++;
		m_char = m_input_file->get();
		return;
	}

	m_token.m_value.pop_back();
	m_token.m_id = *m_token.m_value.begin();
	m_table_of_tokens->push_back(m_token);
	return;
}

void LEXER::Comment() {
	size_t start_line = m_line;
	size_t start_column = m_column;

	m_column++;
	m_char = m_input_file->get();

	if (m_char != '*') {
		std::string error("LEXER ERROR: unidentifier element '(' (line ");
		error += std::to_string(start_line);
		error += ", column ";
		error += std::to_string(start_column);
		error += " )";
		m_table_of_errors->push_back(error);
		return;
	}

	m_column++;
	m_char = m_input_file->get();

	while (true) {
		while (m_char != '*') {
			Whitespace();
			m_char = m_input_file->get();

			if (m_char == -1) {
				std::string error("LEXER ERROR: unclosed comment (line ");
				error += std::to_string(start_line);
				error += ", column ";
				error += std::to_string(start_column);
				error += " )";
				m_table_of_errors->push_back(error);
				return;
			}
		}

		m_column++;
		m_char = m_input_file->get();

		if (m_char == ')') {
			break;
		}
	}
}

void LEXER::Error() {
	std::string error("LEXER ERROR: unidentifier element '");
	error.push_back(m_char);
	error += "' (line ";
	error += std::to_string(m_line);
	error += ", column ";
	error += std::to_string(m_column);
	error += " )";
	m_table_of_errors->push_back(error);
	return;
}

void LEXER::OutputInFile()
{
	*m_generated_file << "|Line\t|Column\t|Id\t|Value\n";

	for (const auto& item : *m_table_of_tokens) {
		*m_generated_file << "|" << item.m_line << "\t|" << item.m_column << "\t|" << item.m_id << "\t|" << item.m_value << "\n";
	}

	for (const auto& item : *m_table_of_errors) {
		*m_generated_file << item << std::endl;
	}
}

TOKEN::TOKEN(size_t line, size_t column, size_t id, std::string& value)
{
	m_line = line;
	m_column = column;
	m_id = id; 
	m_value = value;
}

TOKEN::TOKEN(const TOKEN& token)
{
	m_line = token.m_line;
	m_column = token.m_column;
	m_id = token.m_id;
	m_value = token.m_value;
}
