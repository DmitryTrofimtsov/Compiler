#pragma once
#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <list>

#include "LEXER.h"

class NODE {
public:
	NODE() = default;
	NODE(const std::string& value, int id);
	NODE(const TOKEN& token);

	std::shared_ptr<std::string> m_value;
	int m_id = -1;
	std::shared_ptr<NODE> m_parent;
	std::list<std::shared_ptr<NODE>> m_children;

	size_t m_line;
	size_t m_column;
};

class TREE
{
	std::shared_ptr<NODE> m_current_element;
	std::shared_ptr<NODE> m_head;
	std::shared_ptr<std::ofstream> m_generated_file;

public:
	TREE();

	void Add(const std::string& value, int id);
	void Add(const TOKEN& token);
	void CurrentElem();
	std::shared_ptr<NODE> FindElem(const std::string& value, std::shared_ptr<NODE> cur_elem);
	std::shared_ptr<NODE> GetHead() { return m_head; }

	void PrintTree();
	void PrintNode(const std::shared_ptr<NODE>& node, int depth);
	
	void PrintTreeToFile(const std::string& path, const std::string& error);
	void PrintNodeToFile(const std::shared_ptr<NODE>& node, int depth);
};

