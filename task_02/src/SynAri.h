#ifndef SYN_ARITHMETICAL_H
#define SYN_ARITHMETICAL_H
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stack>
#include "tree_t.h"
#include "Syntax.h"
template<class T>
class Tree
{
public:
	struct Node
	{
		Node(T elem_) :elem(elem_), left(0), right(0)
		{
		}
		Node* left;
		Node* right;
		T elem;
		void in_order_print_helper()
		{
			if (left)
			{
				std::cout << '\t';
				left->in_order_print_helper();
			}
			std::cout << '\t' << elem << '\n';
			if (right)
			{
				std::cout << '\t';
				right->in_order_print_helper();
			}
		}
		int in_order_count_helper()
		{
			int val = 0;
			switch (elem.GetKind())
			{
			case '+':
			{
				val = left->in_order_count_helper() +
					right->in_order_count_helper();
			}
			break;
			case '-':
			{
				val = left->in_order_count_helper() -
					right->in_order_count_helper();
			}
			break;
			case '*':
			{
				val = left->in_order_count_helper() *
					right->in_order_count_helper();
			}
			break;
			case Token::number:
			{
				val = elem.GetNumb();
			}
			}
			return val;
		}
	};
	Tree() :curr(0)
	{
	}
	Node* curr;
	void in_order_print()
	{
		curr->in_order_print_helper();
	}
	int in_order_count()
	{
		return curr->in_order_count_helper();
	}
};

class Token
{
public:
	enum Type {
		add = '+', multi = '*', sub = '-', div = '/',
		number = '8', No
	};
	Token(Type type_ = No, int t_ = 0) :
		type(type_), t(t_)
	{
	}
	const Type GetKind() const
	{
		return type;
	}
	const int GetNumb() const
	{
		return t;
	}
private:
	int t;
	Type type;
};

std::ostream& operator <<(std::ostream& os, const Token& tok)
{
	if (tok.GetKind() != Token::number)
		os << static_cast<char>(tok.GetKind());
	else
		os << tok.GetNumb();
	return os;
}

bool isNumeric(const std::string& some)
{
	int one = 0;
	std::stringstream ss(some);
	ss >> one;
	std::stringstream ss2;
	ss2 << one;
	return (one && some.find_first_not_of(ss2.str()) == std::string::npos);
}

std::string::iterator find_first_in_brakes_helper(std::string& some, const char first, const char second)
{
	std::string tmp;
	tmp += first;
	tmp += second;
	std::string::iterator iter;
	size_t idx = 0;
	if (*some.begin() == '(' && *(some.begin() + 1) == '(')
		idx = some.find_last_of(tmp);
	else
		idx = some.find_first_of(tmp);
	if (idx != std::string::npos)
	{
		iter = some.begin() + idx;
		some.erase(std::find(some.begin(), some.end(), '('));
		some.erase(std::find(some.begin(), some.end(), ')'));
		iter -= 1;
		return iter;
	}
	return some.end();
}

std::pair<std::pair<std::string, std::string>, bool> find_first_in_brakes(std::string& some, const char first, const char second)
{
	if (isNumeric(some))
		return std::make_pair(std::make_pair(some, ""), true);
	std::string::iterator iter = find_first_in_brakes_helper(some, first, second);
	if (iter == some.end())
		return std::make_pair(std::make_pair("", ""), false);
	std::string begin;
	begin.assign(some.begin(), ++iter);
	std::string end;
	end.assign(iter, some.end());
	return std::make_pair(std::make_pair(begin, end), true);
}

std::string::const_iterator find_first_without_brakes_helper(const std::string& some, const char first, const char second)
{
	std::stack<char> lays;
	for (std::string::const_iterator iter = some.begin(); iter != some.end(); ++iter)
	{
		if (*iter == '(')
			lays.push(*iter);
		else if (*iter == ')')
			lays.pop();
		if (lays.empty() && (*iter == first || *iter == second))
		{
			return iter;
		}
	}
	return some.end();
}

std::pair<std::pair<std::string, std::string>, bool> find_first_without_brakes(const std::string& some, const char first, const char second)
{
	if (isNumeric(some))
		return std::make_pair(std::make_pair(some, ""), true);
	std::string::const_iterator iter = find_first_without_brakes_helper(some, first, second);
	if (iter == some.end())
		return std::make_pair(std::make_pair("", ""), false);
	std::string res;
	res.assign(some.begin(), ++iter);
	std::string end;
	end.assign(iter, some.end());
	return std::make_pair(std::make_pair(res, end), true);
}

int toInt(const std::string& some)
{
	int one;
	std::stringstream ss(some);
	ss >> one;
	return one;
}


tree_t* genNode(std::string c) {
	auto* tree = new tree_t{};
	tree->value = c;
	//tree->tk = static_cast<tokens>(c);
	tree->left_node = nullptr;
	tree->rigth_node = nullptr;
	tree->parent_node = nullptr;
	return tree;
}

template<class TernaryFunction>
std::pair<std::string, std::string> create_tree_helper(tree_t** s_tree, std::string& str, TernaryFunction foo)
{
	const std::string opers = "+-*/";
	bool state = false;
	std::string left;
	std::string right;
	for (std::string::const_iterator iter = opers.begin(); iter != opers.end(); iter += 2)
	{
		std::pair<std::pair<std::string, std::string>, bool> pair;
		pair = foo(str, *iter, *(iter + 1));
		if (pair.second == true)
		{
			left = pair.first.first;
			right = pair.first.second;
			char c = '\n';
			if (opers.find(left[left.size() - 1]) != std::string::npos && !left.empty())
			{
				c = left[left.size() - 1];
				std::string tmps{ c };
				*s_tree = genNode(tmps);
				//*s_tree = new Tree<Token>::Node(Token(static_cast<Token::Type>(c), 0));
				left.resize(left.size() - 1);
			}
			std::cout << c << '\n' << left << '\n' << right << '\n';
			std::cout << '\n';
			if (isNumeric(left))
			{
				int tempI = toInt(left);
				(*s_tree)->left_node = genNode(std::to_string(tempI));
				//(*s_tree)->left_node = new Tree<Token>::Node(Token(Token::number, toInt(left)));
				left.clear();
			}
			if (isNumeric(right))
			{
				int tempI = toInt(right);
				(*s_tree)->rigth_node = genNode(std::to_string(tempI));
				//(*s_tree)->rigth_node = new Tree<Token>::Node(Token(Token::number, toInt(right)));
				right.clear();
			}
			state = true;
			break;
		}
		else
			continue;
	}
	return std::make_pair(left, right);
}
bool lays_check(const std::string& str)
{
	std::stack<char> stck;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter)
	{
		if (*iter == '(')
			stck.push(*iter);
		else if (*iter == ')')
			stck.pop();
	}
	return stck.empty();
}
void create_tree(tree_t** s_tree, std::string& str)
{
	if (!lays_check(str))
		throw std::runtime_error("Expression is incorrect");
	std::string left;
	std::string right;
	std::pair<std::string, std::string> left_right = create_tree_helper(s_tree, str, find_first_without_brakes);
	if (left_right.first.empty() && left_right.second.empty() && str.find('(') != std::string::npos)
		left_right = create_tree_helper(s_tree, str, find_first_in_brakes);
	left = left_right.first;
	right = left_right.second;
	if (!left.empty())
		create_tree(&(*s_tree)->left_node, left);
	if (!right.empty())
		create_tree(&(*s_tree)->rigth_node, right);
}



#endif //SYN_ARITHMETICAL_H