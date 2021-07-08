#pragma once
#include "TREE.h"
#include "LEXER.h"

class PARSER
{
	std::unique_ptr<std::list<TOKEN>> m_table_of_tokens;
	std::unique_ptr<TREE> m_tree;

	std::list<TOKEN>::iterator m_current_element;

public:
	PARSER(std::unique_ptr<std::list<TOKEN>>& table_of_tokens);

	void Operation(const std::string& path);

	std::unique_ptr<TREE>& GetTree();

protected:
	void Program();
	void ProcedureIdentifier();
	void Identifier();
	void Block();
	void VariableDeclaration();
	void StatementsList();
	void DeclarationList();
	void Declaration();
	void VariableIdentifier();
	void Statement();
	void ConditionalExpression();
	void LogicalSummand();
	void Logical();
	void LogicalMultiplier();
	void LogicalMultiplierList();
	void ComparisonOperator();
	void Expression();
	void UnsignedInteger();

	void Empty();
	void SetToken();
};

