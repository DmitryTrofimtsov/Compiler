#include "PARSER.h"

PARSER::PARSER(std::unique_ptr<std::list<TOKEN>>& table_of_tokens)
{
	table_of_tokens.swap(m_table_of_tokens);
	m_tree = std::make_unique<TREE>();

	m_current_element = m_table_of_tokens->begin();
}

void PARSER::Operation(const std::string& path)
{
	try {
		Program();
	}
	catch (const std::string& error) {
		throw error;
	}
}

std::unique_ptr<TREE>& PARSER::GetTree()
{
	return m_tree;
}

void PARSER::Program() {

	m_tree->Add("program", 0);
	
	try {
		if (m_current_element->m_value == "PROGRAM") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: 'PROGRAM' expected , but '" + m_current_element->m_value +"' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		ProcedureIdentifier();

		if (m_current_element->m_value == ";") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: ';' expected , but '" + m_current_element->m_value +"' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		Block();

		if (m_current_element->m_value == ".") {
			try {
				SetToken();
			}
			catch (...) {}
		}
		else {
			throw std::string("error: program");
		}
	}
	catch (const std::string& error) {
		throw error;
	}
}

void PARSER::ProcedureIdentifier()
{
	m_tree->Add("procedure-identifier", 0);
	
	try {
		Identifier();
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::Identifier()
{
	m_tree->Add("identifier", 0);

	if (m_current_element->m_id > 1000) {
		SetToken();
		m_tree->CurrentElem();
	}
	else {
		throw std::string("PARSER ERROR: <identifier> expected , but '" + m_current_element->m_value + "' founded (line " 
			+ std::to_string(m_current_element->m_line) + ", column " 
			+ std::to_string(m_current_element->m_column) + ")");
	}
}

void PARSER::Block()
{
	m_tree->Add("block", 0);

	try {
		VariableDeclaration();

		if (m_current_element->m_value == "BEGIN") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: 'BEGIN' expected , but '" + m_current_element->m_value + "' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		StatementsList();

		if (m_current_element->m_value == "END") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: 'END' expected , but '" + m_current_element->m_value + "' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::VariableDeclaration()
{
	m_tree->Add("variable-declaration", 0);

	try {
		if (m_current_element->m_value == "VAR") {
			SetToken();
			DeclarationList();
		}
		else {
			Empty();
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::StatementsList()
{
	m_tree->Add("statement-list", 0);

	try {
		if (m_current_element->m_id > 1000) {
			Statement();
			StatementsList();
		}
		else {
			Empty();
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::DeclarationList()
{
	m_tree->Add("declaration-list", 0);

	try {
		if (m_current_element->m_id > 1000) {
			Declaration();
			DeclarationList();
		}
		else {
			Empty();
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::Declaration()
{
	m_tree->Add("declaration", 0);

	try {
		VariableIdentifier();

		if (m_current_element->m_value == ":") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: ':' expected , but '" + m_current_element->m_value +"' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		if (m_current_element->m_value == "INTEGER") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: 'INTEGER' expected , but '" + m_current_element->m_value + "' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		if (m_current_element->m_value == ";") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: ';' expected , but '" + m_current_element->m_value +"' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::VariableIdentifier()
{
	m_tree->Add("variable-identifier", 0);
	Identifier();
	m_tree->CurrentElem();
}

void PARSER::Statement()
{
	m_tree->Add("statement", 0);

	try {
		VariableIdentifier();

		if (m_current_element->m_value == ":=") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: ':=' expected , but '" + m_current_element->m_value + "' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}

		ConditionalExpression();

		if (m_current_element->m_value == ";") {
			SetToken();
		}
		else {
			throw std::string("PARSER ERROR: ';' expected , but '" + m_current_element->m_value +"' founded (line " 
				+ std::to_string(m_current_element->m_line) + ", column " 
				+ std::to_string(m_current_element->m_column) + ")");
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::ConditionalExpression()
{
	m_tree->Add("conditional-expression", 0);

	try {
		LogicalSummand();
		Logical();
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::LogicalSummand()
{
	m_tree->Add("logical-summand", 0);

	try {
		LogicalMultiplier();
		LogicalMultiplierList();
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::Logical()
{
	m_tree->Add("logical", 0);

	try {
		if (m_current_element->m_value == "OR") {
			SetToken();
			LogicalSummand();
			Logical();
		}
		else {
			Empty();
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::LogicalMultiplier()
{
	m_tree->Add("logical-multiplier", 0);

	try {
		if (m_current_element->m_id > 500) {
			Expression();
			ComparisonOperator();
			Expression();
		}
		else if (m_current_element->m_value == "[") {
			SetToken();
			ConditionalExpression();

			if (m_current_element->m_value == "]") {
				SetToken();
			}
			else {
				throw std::string("PARSER ERROR: ']' expected , but '" + m_current_element->m_value +
					"' founded (line " + std::to_string(m_current_element->m_line) + ", column " 
					+ std::to_string(m_current_element->m_column) + ")");
			}
		}
		else if (m_current_element->m_value == "NOT") {
			SetToken();
			LogicalMultiplier();
		}
		else {
			throw std::string("PARSER ERROR: <expression>, '[' or 'NOT' expected , but '" + m_current_element->m_value +
				"' founded (line " + std::to_string(m_current_element->m_line) + ", column " + std::to_string(m_current_element->m_column));
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::LogicalMultiplierList()
{
	m_tree->Add("logical-multiplier-list", 0);

	try {
		if (m_current_element->m_value == "AND") {
			SetToken();
			LogicalMultiplier();
			LogicalMultiplierList();
		}
		else {
			Empty();
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	m_tree->CurrentElem();
}

void PARSER::ComparisonOperator()
{
	m_tree->Add("comparison-operator", 0);

	if ((m_current_element->m_id > 301 && m_current_element->m_id < 400)
		|| m_current_element->m_value == "<"
		|| m_current_element->m_value == "="
		|| m_current_element->m_value == ">") 
	{
		SetToken();
	}
	else {
		throw std::string("PARSER ERROR: <comparison operator> expected , but '" + m_current_element->m_value +
			"' founded (line " + std::to_string(m_current_element->m_line) + ", column " 
			+ std::to_string(m_current_element->m_column) + ")");
	}

	m_tree->CurrentElem();
}

void PARSER::Expression()
{
	m_tree->Add("expression", 0);

	if (m_current_element->m_id > 1000) {
		VariableIdentifier();
	}
	else if (m_current_element->m_id > 500) {
		UnsignedInteger();
	}
	else {
		throw std::string("PARSER ERROR: <identifier> or <constant> expected , but '" + m_current_element->m_value + 
			"' founded (line " + std::to_string(m_current_element->m_line) + ", column " + std::to_string(m_current_element->m_column) + ")");
	}

	m_tree->CurrentElem();
}

void PARSER::UnsignedInteger()
{
	m_tree->Add("unsigned-integer", 0);

	SetToken();

	m_tree->CurrentElem();
}

void PARSER::Empty()
{
	m_tree->Add("empty", 0);
	m_tree->CurrentElem();
}

void PARSER::SetToken()
{
	m_tree->Add(*m_current_element);
	m_tree->CurrentElem();
	m_current_element++;

	if (m_current_element == m_table_of_tokens->end()) {
		throw std::string("PARSER ERROR: unexpected end of program code");
	}
}
