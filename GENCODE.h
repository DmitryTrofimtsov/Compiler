#pragma once
#include <sstream>
#include "PARSER.h"

class GENCODE
{
	std::unique_ptr<TREE> m_tree;
	std::unique_ptr<std::stringstream> m_generated_code;
	std::unique_ptr<std::ofstream> m_generated_file;

	std::unique_ptr<std::unordered_map<std::string, std::string>> m_table_of_identifications;
	std::vector<std::pair<bool, std::string>> m_table_of_registers;

public:
	GENCODE(std::unique_ptr<TREE>& tree, const std::string& path);
	~GENCODE();


	void TableOfIdentification();
	void AddIdentifierToCode(const std::shared_ptr<NODE>& identifier);

	void Operation();
	void Statement(const std::shared_ptr<NODE>&);
	void ConditionExpression(const std::shared_ptr<NODE>&);
	void LogicalSummand(const std::shared_ptr<NODE>&);
	void Logical(const std::shared_ptr<NODE>&);
	void LogicalMultiplier(const std::shared_ptr<NODE>&);
	void LogicalMultiplierList(const std::shared_ptr<NODE>&);
	
	void LogicalMultiplier_Branch1(const std::shared_ptr<NODE>&);
	void LogicalMultiplier_Branch2(const std::shared_ptr<NODE>&);

	void ExpressionIdentifier(const std::shared_ptr<NODE>&, const std::string& );
};

