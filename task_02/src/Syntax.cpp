//
// Created by vovan on 28.09.2019.
//

#include "Syntax.h"

/**
=======
 * TODO: Update grammar for your variant of tasks
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> [ = <exp> ] ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <condoper> |
 * <cond_oper>	 ::= <logical_expr> then <compound> [ else <compund> ]
 * <logical_expr>::= <sing_logic_expr> 
 * <sing_logic_expr> ::= <logic_term> | <sing_logic_expr> or <logic_term>
 * <logic_term>  ::= <logic_mult> | <logic_term> and <logic_mult>
 * <logic_mult>  ::= <logic_const> | <logic_id> | not <logic_mult> | (<logic_expr>)
 * <logic_string_relation> ::= <id>|<exp>|<constant> <logic_op> <id>|<exp>|<constant>
 * <logic_op>    ::= = | <> | <= | < | >= | >
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant> | <your_other_operations>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */

Syntax::Syntax(std::vector<Lexem> &&t_lex_table) {
    if (t_lex_table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty");
    if (t_lex_table.at(0).GetToken() == eof_tk)
        throw std::runtime_error("<E> Syntax: Code file is empty");
    lex_table = t_lex_table;
    cursor    = lex_table.begin();
}


Syntax::~Syntax() {
	freeTreeNode(pascal_tree);
}

tree_t Syntax::getPascal_tree()
{
	return *pascal_tree;
}


/**
 * XXX: Each block (..Parse) builds its subtree (and returns it),
 *   which is then added at a higher level, at the place of the method call.
 */


/**
 * @brief Start parse incoming pascal file
 * @param none
 *
 * @return  EXIT_SUCCESS - if file was successful parsed
 * @return -EXIT_FAILURE - if can't parse incoming file
 */
int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto &it = cursor;
    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while(it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);
    std::cout << "EOF" << std::endl;

    return EXIT_SUCCESS;
}


/**
 * @brief Parse entry point in grammar
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if input program part is correct
 * @return -EXIT_FAILURE - if input program part is incorrect
 * @note Wait input like: program <id_tk> ;
 */
int Syntax::programParse(lex_it &t_iter) {
    if (!checkLexem(t_iter, program_tk)) {
        printError(MUST_BE_PROG, *t_iter);
        return -EXIT_FAILURE;
    }

    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        } else {
            printError(MUST_BE_ID, *iter);
            return -EXIT_FAILURE;
        }
    }
    auto root_name = iter->GetName(); // save the name of program

    iter = getNextLex(t_iter);
    if (!checkLexem(iter, semi_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        } else {
            printError(MUST_BE_SEMI, *iter);
            return -EXIT_FAILURE;
        }
    }

    // First phase is OK, we can start to build the tree
    pascal_tree = createNode(root_name); // TODO: rewrite, set unifications
                                         //   for all *Parse methods

    return EXIT_SUCCESS;
}


/**
 * @brief Parse block part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if block part is matched to grammar
 * @return -EXIT_FAILURE - if block part doesn't matched to grammar
 */
int Syntax::blockParse(lex_it& t_iter, tree_t* t_tree) {
	try {
		auto iter = getNextLex(t_iter);
		switch (iter->GetToken()) {
		case var_tk: {
			pascal_tree->left_node = buildTreeStub(pascal_tree, iter->GetName());
			vardpParse(t_iter, pascal_tree->left_node);
			break;
		}
		case begin_tk: {
			pascal_tree->rigth_node = buildTreeStub(pascal_tree, iter->GetName());
			pascal_tree->rigth_node->rigth_node = compoundParse(t_iter, pascal_tree->rigth_node->rigth_node);
			break;
		}
		case dot_tk: {
			std::cout << "Program was parse successfully" << std::endl;
			break;
		}
		default: {
			break;
		}
		}
	}
	catch (const std::exception & exp) {
		std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
			<< exp.what() << std::endl;
		return -EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/**
 * @brief Parse variable declaration part
 * @param[inout] t_iter - iterator of table of lexeme
 * @param[inout] t_tree - subtree of part of variables
 *
 * @return  EXIT_SUCCESS - if variable declaration part is matched to grammar
 * @return -EXIT_FAILURE - if variable declaration part doesn't matched to grammar
 */
int Syntax::vardpParse(Syntax::lex_it &t_iter, tree_t *t_tree) {
    // var_list contains list of variables from current code line
    auto var_list = vardParse(t_iter);
    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);
    if (!checkLexem(t_iter, type_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        printError(MUST_BE_SEMI, *t_iter);
    }

    updateVarTypes(var_list, type_iter->GetName());
    buildVarTree(var_list, t_tree);

    auto forwrd_lex = peekLex(1, t_iter);
    if (checkLexem(forwrd_lex, var_tk) || checkLexem(forwrd_lex, id_tk)) {
        if (checkLexem(forwrd_lex, var_tk))
            getNextLex(t_iter);
        vardpParse(t_iter, t_tree->rigth_node);
    } else {
        freeTreeNode(t_tree->rigth_node->rigth_node);
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse line of variables
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return list of variables
 * @note For example, we have next code lines:
 *   program exp1;
 *   var
 *     a, b : integer;    // return { a, b }
 *     c, d, e : integer; // return { c, d, e }
 *   ...
 */
std::list<std::string> Syntax::vardParse(lex_it &t_iter) {
    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        printError(MUST_BE_ID, *iter);
        return std::list<std::string>();
    }

    if (isVarExist(iter->GetName())) printError(DUPL_ID_ERR, *iter);
    else
        id_map.emplace(iter->GetName(), Variable("?", "?"));

    std::list<std::string> var_list;
    var_list.push_back(t_iter->GetName());

    iter = getNextLex(t_iter);
    if (checkLexem(iter, comma_tk))
        var_list.splice(var_list.end(), vardParse(t_iter));

    return var_list;
}


/**
 * @brief Parse compound part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if compound part is matched to grammar
 * @return -EXIT_FAILURE - if compound part doesn't matched to grammar
 */
tree_t* Syntax::compoundParse(lex_it& t_iter, tree_t* t_tree) {
	static int compound_count = 0; // XXX: How can this be replaced?
	static tree_t** tmpTree = &t_tree;
	compound_count++;
	int state_count = 0;
	std::string op = "op";
	op = op + std::to_string(compound_count) + "_" + std::to_string(state_count + 1);
	t_tree = buildTreeStub(t_tree, op);	 
	while (t_iter->GetToken() != end_tk) {
		if (t_iter->GetToken() == eof_tk) {
			printError(EOF_ERR, *t_iter);
			return nullptr;
		}
		state_count++;
		if (state_count > 1) {
			op.pop_back();
			op += std::to_string(state_count);
			t_tree->rigth_node = buildTreeStub(t_tree, op);
			t_tree = t_tree->rigth_node;
		}

		stateParse(t_iter,&t_tree->left_node);
	}

	if (compound_count == 1) { // XXX: How can this be replaced?
		if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
			checkLexem(peekLex(1, t_iter), eof_tk)) {
			printError(MUST_BE_DOT, *t_iter);
			return nullptr;
		}
	}
	t_tree->value = "end";
	while (t_tree->value[t_tree->value.size() - 1] != '1') {
		t_tree = t_tree->parent_node;
	}
	return t_tree;
}


/**
 * @brief Parse state part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
tree_t* Syntax::stateParse(lex_it& t_iter, tree_t** t_tree) {
	auto iter = getNextLex(t_iter);
	std::string tmpS;
	switch (iter->GetToken()) {
	case id_tk: {
		if (!isVarExist(iter->GetName())) {
			printError(UNKNOWN_ID, *t_iter);
			return nullptr;
		}
		tmpS = iter->GetName();
		getNextLex(t_iter);
		if (!checkLexem(t_iter, ass_tk)) {
			printError(MUST_BE_ASS, *t_iter);
			return nullptr;
		}
		*t_tree = buildTreeStub(*t_tree, ":=");
		(*t_tree)->left_node = buildTreeStub(*t_tree, tmpS);
		(*t_tree)->rigth_node = expressionParse(t_iter,"", &(*t_tree)->rigth_node);
		if (!checkLexem(t_iter, semi_tk)) { // we exit from expression on the ';'
			printError(MUST_BE_SEMI, *t_iter);
			return nullptr;
		}
		break;
	}
	case begin_tk: {
		*t_tree = buildTreeStub(*t_tree, "begin");
		(*t_tree)->rigth_node = compoundParse(t_iter,(*t_tree)->rigth_node);
		getNextLex(t_iter);
		if (!checkLexem(t_iter, semi_tk)) {
			printError(MUST_BE_SEMI, *t_iter);
			return nullptr;
		}
		break;
	}
	default: {
		break;
	}
	}

	return *t_tree;
}


/**
 * @brief Parse expression part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */
tree_t* Syntax::expressionParse(lex_it& t_iter,std::string tmp, tree_t** t_tree) {
	auto iter = getNextLex(t_iter);
	static std::string k = "";
	static bool flagl = false;
	switch (iter->GetToken()) {
	case id_tk: {
		if (!isVarExist(iter->GetName()))
			printError(UNKNOWN_ID, *t_iter);
		if (valueOfID(iter->GetName()) == "")
			return nullptr;
	}
	case constant_tk: {
		if(iter->GetToken() == id_tk)
			k += valueOfID(iter->GetName());
		else if (iter->GetToken() == constant_tk)
			k += iter->GetName();
		iter = getNextLex(t_iter);
		if (iter->GetName() == ";" && flagl == false)
		{
			*t_tree = createNode(k);
			k = "";
			return *t_tree;
		}
		switch (iter->GetToken()) {
		case add_tk:
		case sub_tk:
		case mul_tk:
		case div_tk: {
			if (iter->GetToken() == div_tk)
				k += "/";
			else
				k += iter->GetName();
			flagl = true;
			expressionParse(t_iter, k,t_tree);
			break;
		}
		default: { 
			break;
		}
		}

		break;
	}
	case sub_tk: {
		k += iter->GetName();
		auto tmps = *(peekLex(1, iter));
		if (tmps.GetToken() == constant_tk) {
			iter = getNextLex(t_iter);
			iter = getNextLex(t_iter);
			k += tmps.GetName();
			*t_tree = createNode(k);
			k = "";
			return *t_tree;
		}
		break;
	}
	default: {
		printError(MUST_BE_ID, *t_iter);
		return nullptr;
	}
	}

	
	if (k != "") {
		create_tree(t_tree, k);
		k = "";
		flagl = false;
		return *t_tree;
	}
	return *t_tree;

}


/**
 * @brief Create node of syntax tree
 * @param[in] t_tree    - parent node
 * @param[in] node_name - name of the created node (value)
 *
 * @return node of tree
 */
tree_t *Syntax::buildTreeStub(tree_t *t_tree, const std::string &node_name) {
    auto *tree        = new tree_t;
    tree->value       = node_name;
    tree->parent_node = std::addressof(*t_tree); // get address of parent node
    tree->left_node   = nullptr;
    tree->rigth_node  = nullptr;

    return tree;
}


/**
 * @brief Create like a root node of syntax tree
 * @param[in] node_name - name of the node (value)
 *
 * @return node of tree
 */
tree_t *Syntax::createNode(const std::string &node_name) {
    auto *tree        = new tree_t;
    tree->value       = node_name;
    tree->left_node   = nullptr;
    tree->rigth_node  = nullptr;
    tree->parent_node = nullptr;

    return tree;
}


/**
 * @brief Print information about error
 * @param[in] t_err - error type
 * @param[in] lex   - error lexeme
 *
 * @return none
 */
void Syntax::printError(errors t_err, Lexem lex) {
	switch (t_err) {
	case UNKNOWN_LEXEM: {
		std::cerr << "<E> Lexer: Get unknown lexem '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case EOF_ERR: {
		std::cerr << "<E> Syntax: Premature end of file" << std::endl;
		break;
	}
	case MUST_BE_ID: {
		std::cerr << "<E> Syntax: Must be identifier instead '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case MUST_BE_SEMI: {
		std::cerr << "<E> Syntax: Must be ';' instead '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case MUST_BE_PROG: {
		std::cerr << "<E> Syntax: Program must start from lexem 'program' ("
			<< lex.GetLine() << ")" << std::endl;
		break;
	}
	case MUST_BE_COMMA: {
		std::cerr << "<E> Syntax: Must be ',' instead '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case DUPL_ID_ERR: {
		std::cerr << "<E> Syntax: Duplicate identifier '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case UNKNOWN_ID: {
		std::cerr << "<E> Syntax: Undefined variable '" << lex.GetName()
			<< "' on " << lex.GetLine() << " line" << std::endl;
		break;
	}
	case MUST_BE_DOT: {
		std::cerr << "<E> Syntax: Program must be end by '.'" << std::endl;
		break;
	}
	case OPEN_ERR: {
		std::cerr << "<E> Syntax: could not open file" << std::endl;
		break;
	}
	case WRONG_VAR: {
		std::cerr << "<E> Syntax: variable not found" << std::endl;
		break;
	}
					// TODO: Add remaining error types
	default: {
		std::cerr << "<E> Syntax: Undefined type of error" << std::endl;
		break;
	}
	}
}


/**
 * @brief Get next lexeme
 * @param[inout] iter - cursor-iterator of lexeme table
 *
 * @return iterator on next lexeme
 */
Syntax::lex_it Syntax::getNextLex(lex_it &iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }

    return iter;
}


/**
 * @brief Peek to forward on the N lexeme
 * @param[in] N      - the number of skipped tokens
 * @param[in] t_iter - copy of cursor-iterator of lexeme table
 *
 * @return copy of iterator on N-th lexeme (token)
 * @note If catch exception, return copy of iterator
 */
Syntax::lex_it Syntax::peekLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter++; N--;
        }

        return iter;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Can't peek so forward" << std::endl;
        return t_iter;
    }
}


/**
 * @brief Check does current lexeme is match to needed token
 * @param[in] t_iter - current lexem
 * @param[in] t_tok  - needed token
 *
 * @return true  - if lexeme is match
 * @return false - if lexeme is unreachable (end) or if doesn't match
 */
bool Syntax::checkLexem(const Syntax::lex_it &t_iter, const tokens &t_tok) {
    if (t_iter == lex_table.end())   return false;
    if (t_iter->GetToken() != t_tok) return false;

    return true;
}


/**
 * @brief Check existence of variable
 * @param[in] t_var_name - variable for check
 *
 * @return true  - if variable is exist
 * @return false - if unknown variable (doesn't exist)
 */

  
bool Syntax::isVarExist(const std::string& t_var_name) {
	auto map_iter = id_map.find(t_var_name);
	return !(map_iter == id_map.end());
}


/**
 * @brief Update information about type in map of identifiers
 * @param[in] t_var_list  - list of variables
 * @param[in] t_type_name - type of variables
 *
 * @return none
 */
void Syntax::updateVarTypes(const std::list<std::string> &t_var_list,
                            const std::string &t_type_name) {
    try {
        for (auto &el: t_var_list)
            id_map.at(el).type = t_type_name;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}


/**
 * @brief Build subtree of variable declaration part
 * @param[in]  t_var_list - list of variable
 * @param[out] t_tree     - subtree of variable part
 *
 * @return none
 * @note If we firstly call this method:
 *                              program_name
 *                               /         \
 *             t_tree here ->  var         <block>
 */
void Syntax::buildVarTree(const std::list<std::string> &t_var_list, tree_t *t_tree) {
    try {
        auto i = 0;
        for (auto &el: t_var_list) {
            tree_t *tmp_tree     = createNode(el);
            tmp_tree->rigth_node = buildTreeStub(tmp_tree, id_map.at(el).type);
            createVarTree(t_tree, tmp_tree, i++);
        }
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}


/**
 * @brief Insert subtree of <var dec> part to tree <var part>
 * @param[out] t_tree       - current node (look on var/$ root)
 * @param[in]  t_donor_tree - tree with information about identifier
 * @param[in]  lvl          - level of recursion
 *
 * @return none
 * @note How look t_tree:
 *                      program_name
 *                       /      \
 *                     var     <block>
 *                     / \
 *       <t_donor_tree>  <t_tree>
 *                       / \
 *         <t_donor_tree>  $
 *                        etc.
 * How look t_donor_tree:
 *                  a           <id>
 *                   \             \
 *                   integer       <type>
 */
void Syntax::createVarTree(tree_t *t_tree, tree_t *t_donor_tree, int lvl) {
    if (lvl > 0) {
        lvl--;
        createVarTree(t_tree->rigth_node, t_donor_tree, lvl);
    } else {
        t_tree->left_node  = t_donor_tree;
        t_tree->rigth_node = buildTreeStub(t_tree,"$");
    }
}


/**
 * @brief Free allocated (deallocate) memory
 * @param[in] t_tree - node of tree
 *
 * @return none
 */
void Syntax::freeTreeNode(tree_t*& t_tree) {
	try {
		if (t_tree->left_node != nullptr) freeTreeNode(t_tree->left_node);
		if (t_tree->rigth_node != nullptr) freeTreeNode(t_tree->rigth_node);
		delete t_tree;
		t_tree = nullptr;
	}
	catch (const std::exception & exp) {
		std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
			<< exp.what() << std::endl;
	}
}

bool Syntax::isNumeric(const std::string& some)
{
	int one = 0;
	std::stringstream ss(some);
	ss >> one;
	std::stringstream ss2;
	ss2 << one;
	return (one && some.find_first_not_of(ss2.str()) == std::string::npos);
}

std::string::iterator Syntax::find_first_in_brakes_helper(std::string& some, const char first, const char second)
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

std::pair<std::pair<std::string, std::string>, bool> Syntax::find_first_in_brakes(std::string& some, const char first, const char second)
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

std::string::const_iterator Syntax::find_first_without_brakes_helper(const std::string& some, const char first, const char second)
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

std::pair<std::pair<std::string, std::string>, bool> Syntax::find_first_without_brakes(const std::string& some, const char first, const char second)
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

tree_t* Syntax::genNode(std::string c)
{
	auto* tree = new tree_t{};
	tree->value = c;
	//tree->tk = static_cast<tokens>(c);
	tree->left_node = nullptr;
	tree->rigth_node = nullptr;
	tree->parent_node = nullptr;
	return tree;
}

std::pair<std::string, std::string> Syntax::create_tree_helper_1(tree_t** s_tree, std::string& str)
{
	const std::string opers = "+-*/";
	bool state = false;
	std::string left;
	std::string right;
	for (auto iter = opers.begin(); iter != opers.end(); iter += 2)
	{
		std::pair<std::pair<std::string, std::string>, bool> pair;
		pair = find_first_without_brakes(str, *iter, *(iter + 1));
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
				(*s_tree)->left_node = genNode(left);
				//(*s_tree)->left_node = new Tree<Token>::Node(Token(Token::number, toInt(left)));
				left.clear();
			}
			if (isNumeric(right))
			{
				(*s_tree)->rigth_node = genNode(right);
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

std::pair<std::string, std::string> Syntax::create_tree_helper_2(tree_t** s_tree, std::string& str)
{
	const std::string opers = "+-*/";
	bool state = false;
	std::string left;
	std::string right;
	for (std::string::const_iterator iter = opers.begin(); iter != opers.end(); iter += 2)
	{
		std::pair<std::pair<std::string, std::string>, bool> pair;
		pair = find_first_in_brakes(str, *iter, *(iter + 1));
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
				(*s_tree)->left_node = genNode(left);
				//(*s_tree)->left_node = new Tree<Token>::Node(Token(Token::number, toInt(left)));
				left.clear();
			}
			if (isNumeric(right))
			{
				(*s_tree)->rigth_node = genNode(right);
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

bool Syntax::lays_check(const std::string& str)
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
void Syntax::create_tree(tree_t** s_tree, std::string& str)
{
	if (!lays_check(str))
		throw std::runtime_error("Expression is incorrect");
	std::string left;
	std::string right;
	std::pair<std::string, std::string> left_right = create_tree_helper_1(s_tree, str);
	if (left_right.first.empty() && left_right.second.empty() && str.find('(') != std::string::npos)
		left_right = create_tree_helper_2(s_tree, str);
	left = left_right.first;
	right = left_right.second;
	if (!left.empty())
		create_tree(&(*s_tree)->left_node, left);
	if (!right.empty())
		create_tree(&(*s_tree)->rigth_node, right);
}

std::string Syntax::valueOfID(std::string tmp)
{
	auto tm = id_map.at(tmp);
	if (tm.value != "?")
		return tm.value;
	else
		return "";
}

