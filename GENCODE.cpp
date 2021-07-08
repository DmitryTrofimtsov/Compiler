#include "GENCODE.h"

GENCODE::GENCODE(std::unique_ptr<TREE>& tree, const std::string& path)
{
	m_tree = std::make_unique<TREE>();
	m_tree.swap(tree);
	m_table_of_identifications = std::make_unique<std::unordered_map<std::string, std::string>>();
	m_generated_file = std::make_unique<std::ofstream>(path + "/generated.txt");
	m_generated_code = std::make_unique<std::stringstream>();

	m_table_of_registers = std::vector<std::pair<bool, std::string>>(4);
	m_table_of_registers.at(0) = std::make_pair(true, "eax");
	m_table_of_registers.at(1) = std::make_pair(true, "ebx");
	m_table_of_registers.at(2) = std::make_pair(true, "ecx");
	m_table_of_registers.at(3) = std::make_pair(true, "edx");

	*m_generated_code << "push rbp\nmov rbp, rsp\n\n";

	try {
		TableOfIdentification();
	}
	catch (const std::string& error) {
		*m_generated_file << error << std::endl;
		return;
	}
	
	*m_generated_code << "\n";

	try {
		Operation();
	}
	catch (const std::string& error) {
		*m_generated_file << error << std::endl;
		return;
	}

	*m_generated_code << "pop rbp\nret\n";

	*m_generated_file << m_generated_code->str();
}

GENCODE::~GENCODE()
{
	if (m_generated_file->is_open()) {
		m_generated_file->close();
	}
}

void GENCODE::TableOfIdentification()
{
	auto proc_identifier = m_tree->FindElem("identifier", m_tree->GetHead());
	m_table_of_identifications->insert({*proc_identifier->m_children.front()->m_value, "empty"});

	auto declaration_list = m_tree->FindElem("declaration-list", m_tree->GetHead());
	
	while (*declaration_list->m_children.front()->m_value != "empty") {
		auto identifier = m_tree->FindElem("identifier", declaration_list);
		identifier = identifier->m_children.front();

		if (m_table_of_identifications->find(*identifier->m_value) == m_table_of_identifications->end()) {
			AddIdentifierToCode(identifier);
		}
		else {
			//error
			throw std::string("GENCODE ERROR: identifier '" + *identifier->m_value + "' already exist. Line: "
				+ std::to_string(identifier->m_line) + ", column: " + std::to_string(identifier->m_column));
		}

		declaration_list = m_tree->FindElem("declaration-list", declaration_list);
	}
}

void GENCODE::AddIdentifierToCode(const std::shared_ptr<NODE>& identifier)
{
	static size_t shift = 4;

	std::ostringstream tmp;
	tmp << "mov DWORD PTR[rbp-" << shift << "]";
	shift += 4;
	
	m_table_of_identifications->insert({ *identifier->m_value, tmp.str() });
	*m_generated_code << tmp.str() << ", 0\n";
}

void GENCODE::Operation() {
	auto statement_list = m_tree->FindElem("statement-list", m_tree->GetHead());

	while (*statement_list->m_children.front()->m_value != "empty") {
		try {
			Statement(statement_list->m_children.front());
		}
		catch (const std::string& error) {
			throw error;
		}
		
		*m_generated_code << "\n";
		statement_list = m_tree->FindElem("statement-list", statement_list);
	}
}

void GENCODE::Statement(const std::shared_ptr<NODE>& statement)
{
	auto variable_identifier = m_tree->FindElem("identifier", statement->m_children.front());

	//if identifier does not exis in table of identifier
	if (m_table_of_identifications->find(*variable_identifier->m_children.front()->m_value) == m_table_of_identifications->end()) {
		//error
		throw std::string("GENCODE ERROR: identifier '" + *variable_identifier->m_children.front()->m_value + "' is undefined. Line: "
			+ std::to_string(variable_identifier->m_children.front()->m_line) + ", column: "
			+ std::to_string(variable_identifier->m_children.front()->m_column));
		return;
	}

	//if identifier is "procedure-identifier"
	if (m_table_of_identifications->find(*variable_identifier->m_children.front()->m_value)->second == "empty") {
		//error
		throw std::string("GENCODE ERROR: identifier '" + *variable_identifier->m_children.front()->m_value + "' is undefined. Line: "
			+ std::to_string(variable_identifier->m_children.front()->m_line) + ", column: "
			+ std::to_string(variable_identifier->m_children.front()->m_column));
		return;
	}

	try {
		ConditionExpression(m_tree->FindElem("conditional-expression", statement));
	}
	catch (const std::string& error) {
		throw error;
	}

	//mov "eax" to "variable-identifier"
	*m_generated_code << "mov " << m_table_of_identifications->find(*variable_identifier->m_children.front()->m_value)->second << ", eax\n";
	m_table_of_registers.at(0).first = true;
	m_table_of_registers.at(1).first = true;
	m_table_of_registers.at(2).first = true;
	m_table_of_registers.at(3).first = true;
}

void GENCODE::ConditionExpression(const std::shared_ptr<NODE>& conditional_expression)
{
	try {
		LogicalSummand(conditional_expression->m_children.front());
		Logical(*(++conditional_expression->m_children.begin()));
	}
	catch (const std::string& error) {
		throw error;
	}
}

void GENCODE::LogicalSummand(const std::shared_ptr<NODE>& logical_summand)
{
	LogicalMultiplier(logical_summand->m_children.front());
	LogicalMultiplierList(*(++logical_summand->m_children.begin()));
}

void GENCODE::Logical(const std::shared_ptr<NODE>& logical) {

	if (*logical->m_children.front()->m_value == "OR") {

		try {
			LogicalSummand(*(++logical->m_children.begin()));
			Logical(*++((++logical->m_children.begin())));
		}
		catch (const std::string& error) {
			throw error;
		}

		for (size_t i = 1; i < 4; ++i) {
			if (m_table_of_registers.at(i).first == false) {
				*m_generated_code << "OR eax, " << m_table_of_registers.at(i).second << "\n";
				m_table_of_registers.at(i).first = true;
				break;
			}
		}
	}

}

void GENCODE::LogicalMultiplier(const std::shared_ptr<NODE>& logical_multiplier)
{
	try {
		if (*logical_multiplier->m_children.front()->m_value == "expression") {
			LogicalMultiplier_Branch1(logical_multiplier);
		}
		else if (*logical_multiplier->m_children.front()->m_value == "NOT") {
			LogicalMultiplier_Branch2(logical_multiplier);
		}
		else {
			ConditionExpression(m_tree->FindElem("conditional-expression", logical_multiplier));
		}
	}
	catch (const std::string& error) {
		throw error;
	}
}

void GENCODE::LogicalMultiplierList(const std::shared_ptr<NODE>& logical_multiplier_list)
{
	if (*logical_multiplier_list->m_children.front()->m_value == "AND") {
		try {
			LogicalMultiplier(*(++logical_multiplier_list->m_children.begin()));
			LogicalMultiplierList(*(++(++logical_multiplier_list->m_children.begin())));
		}
		catch (const std::string& error) {
			throw error;
		}

		for (size_t i = 1; i < 4; ++i) {
			if (m_table_of_registers.at(i).first == false) {
				*m_generated_code << "AND eax, " << m_table_of_registers.at(i).second << "\n";
				m_table_of_registers.at(i).first = true;
				break;
			}
		}
	}
}

void GENCODE::LogicalMultiplier_Branch1(const std::shared_ptr<NODE>& logical_multiplier)
{
	auto expression_1 = logical_multiplier->m_children.front();
	auto expression_2 = logical_multiplier->m_children.back();
	
	auto elem_1 = expression_1->m_children.front();
	auto elem_2 = expression_2->m_children.front();

	std::vector<std::pair<bool, std::string>>::iterator reg_1;
	std::vector<std::pair<bool, std::string>>::iterator reg_2;

	try {	
		for (auto reg = m_table_of_registers.begin(); reg != m_table_of_registers.end(); reg++) {
			if (reg->first == true) {
				reg_1 = reg;
				ExpressionIdentifier(expression_1->m_children.front(), reg_1->second);
				reg_1->first = false;
				break;
			}
		}
		for (auto reg = m_table_of_registers.begin(); reg != m_table_of_registers.end(); reg++) {
			if (reg->first == true) {
				reg_2 = reg;
				ExpressionIdentifier(expression_2->m_children.front(), reg_2->second);
				reg_2->first = false;
				break;
			}
		}
	}
	catch (const std::string& error) {
		throw error;
	}

	*m_generated_code << "cmp " << reg_1->second << ", " << reg_2->second << "\n";

	auto comparison_operator = (*(++logical_multiplier->m_children.begin()))->m_children.front();

	switch (comparison_operator->m_id) {
	case 302:
		*m_generated_code << "jle ";
		break;

	case 303:
		*m_generated_code << "jge ";
		break;

	case 304:
		*m_generated_code << "jne ";
		break;

	case static_cast<int>('>'):
		*m_generated_code << "jg ";
		break;

	case static_cast<int>('='):
		*m_generated_code << "je ";
		break;

	case static_cast<int>('<') :
		*m_generated_code << "jl ";
		break;
	}

	static int label = 1;
	*m_generated_code << ".Label" << label++ << "\n";

	*m_generated_code << "mov " << reg_1->second << ", 0\njmp .Label" << label << "\n";
	*m_generated_code << ".Label" << label - 1 << ":\n";
	*m_generated_code << "mov " << reg_1->second << ", 1\n";
	*m_generated_code << ".Label" << label++ << ":\n";

	reg_2->first = true;
}

void GENCODE::ExpressionIdentifier(const std::shared_ptr<NODE>& expression, const std::string& reg)
{

	if (*expression->m_value == "variable-identifier") {
		auto identifier = m_table_of_identifications->find(*expression->m_children.front()->m_children.front()->m_value);

		//if "variable-identifier" does not exist
		if (identifier == m_table_of_identifications->end()) {
			throw std::string("GENCODE ERROR: identifier '" + *expression->m_children.front()->m_children.front()->m_value + "' is undefined. Line: " 
				+ std::to_string(expression->m_children.front()->m_children.front()->m_line) + ", column: " 
				+ std::to_string(expression->m_children.front()->m_children.front()->m_column) + ")");
		}

		//if "variable-identifier" does not exist
		if (identifier->second == "empty") {
			throw std::string("GENCODE ERROR: identifier '" + *expression->m_children.front()->m_children.front()->m_value + "' is undefined. Line: "
				+ std::to_string(expression->m_children.front()->m_children.front()->m_line) + ", column: "
				+ std::to_string(expression->m_children.front()->m_children.front()->m_column) + ")");
		}

		*m_generated_code << "mov " << reg << ", " << identifier->second << "\n";
	}
	else {
		*m_generated_code << "mov " << reg << ", " << *expression->m_children.front()->m_value << "\n";
	}
}

void GENCODE::LogicalMultiplier_Branch2(const std::shared_ptr<NODE>& logical_multiplier)
{
	LogicalMultiplier(*(++logical_multiplier->m_children.begin()));
	*m_generated_code << "NOT eax\n";
}
