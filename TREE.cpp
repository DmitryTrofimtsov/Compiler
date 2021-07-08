#include "TREE.h"

NODE::NODE(const std::string& value, int id) {
	m_id = id;
	m_value = std::make_shared<std::string>(value);
}

NODE::NODE(const TOKEN& token)
{
	m_id = token.m_id;
	m_value = std::make_shared<std::string>(token.m_value);
	m_line = token.m_line;
	m_column = token.m_column;
}

TREE::TREE()
{
	m_head = std::make_shared<NODE>();
	m_current_element = m_head;
	m_head->m_value = std::make_shared<std::string>("signal-program");
	m_head->m_id = 0;
	m_head->m_parent = nullptr;

	m_generated_file = std::make_shared<std::ofstream>();
}

void TREE::Add(const std::string& value, int id)
{
	auto NewNode = std::make_shared<NODE>(value, id);
	NewNode->m_parent = m_current_element;
	m_current_element->m_children.push_back(NewNode);
	m_current_element = NewNode;
}

void TREE::Add(const TOKEN& token)
{
	auto NewNode = std::make_shared<NODE>(token);
	NewNode->m_parent = m_current_element;
	m_current_element->m_children.push_back(NewNode);
	m_current_element = NewNode;
}

void TREE::CurrentElem()
{
	m_current_element = m_current_element->m_parent;
}

std::shared_ptr<NODE> TREE::FindElem(const std::string& value, std::shared_ptr<NODE> cur_elem)
{
	for (const auto& item : cur_elem->m_children) {
		if (*item->m_value == value) {
			return item;
		}
	}

	for (const auto& item : cur_elem->m_children) {
		auto res = FindElem(value, item);

		if (res->m_id != -1) {
			return res;
		}
	}

	return std::make_shared<NODE>();
}

void TREE::PrintTree()
{
	if (m_head != nullptr) {
		PrintNode(m_head, 0);
	}
}

void TREE::PrintNode(const std::shared_ptr<NODE>& node, int depth)
{
	for (int i = -1; i < depth; ++i) {
		std::cout << "--";
	}

	if (node->m_id == 0) {
		std::cout << " <" << *(node->m_value) << ">\n";
	}
	else {
		std::cout << " " << node->m_id << "   " << *(node->m_value) << std::endl;
	}

	for (auto& item : node->m_children) {
		PrintNode(item, depth + 1);
	}
}

void TREE::PrintTreeToFile(const std::string& path, const std::string& error)
{
	m_generated_file->open(path + "/generated.txt");

	if (m_head != nullptr) {
		PrintNodeToFile(m_head, 0);

		if (!error.empty()) {
			*m_generated_file << error;
		}
	}
}

void TREE::PrintNodeToFile(const std::shared_ptr<NODE>& node, int depth)
{
	for (int i = -1; i < depth; ++i) {
		*m_generated_file << "--";
	}

	if (node->m_id == 0) {
		*m_generated_file << " <" << *(node->m_value) << ">\n";
	}
	else {
		*m_generated_file << " " << node->m_id << "   " << *(node->m_value) << std::endl;
	}

	for (auto& item : node->m_children) {
		PrintNodeToFile(item, depth + 1);
	}
}
