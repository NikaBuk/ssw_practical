//
// Created by vovan on 28.09.2019.
//

#include "Syntax.h"
/**
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> [ = <exp> ] ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <condoper> |
 * <cond_oper>   ::= <logical_expr> then <compound> [ else <compund> ]
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

    operations.emplace(":=", 0);
    operations.emplace("if", 0);
    operations.emplace("for", 0);

    operations.emplace("=",  1);
    operations.emplace("<>", 1);
    operations.emplace("<",  1);
    operations.emplace(">",  1);
    operations.emplace("<=", 1);
    operations.emplace(">=", 1);

    operations.emplace("+",   2);
    operations.emplace("-",   2);
    operations.emplace("or",  2);
    operations.emplace("xor", 2);

    operations.emplace("*",   3);
    operations.emplace("and", 3);
    operations.emplace("/",   3);
    operations.emplace("div", 3);
}


Syntax::~Syntax() {
    Tree::FreeTree(root_tree);
}


/**
 * Каждый блок (..Parse) строит своё поддерево (и возвращает его),
 *  которое затем добавляется на уровне выше, в месте вызова метода.
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

    while (it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);

    std::cout << std::endl;
    std::cout << std::setfill('*') << std::setw(50);
    std::cout << "\r\n";

    //root_tree->PrintTree();
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
    root_tree = Tree::CreateNode(root_name); // TODO: rewrite, set unifications
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
int Syntax::blockParse(lex_it &t_iter) {
    try {
        auto iter = getNextLex(t_iter);

        switch (iter->GetToken()) {
            case var_tk: {
                root_tree->AddLeftNode("var");
                vardpParse(t_iter, root_tree->GetLeftNode());
                break;
            }

            case begin_tk: {
                // TODO: add check on nullptr from compoundParse
                Tree* tmpTree = new Tree();
                tmpTree = compoundParse(t_iter);

                if (tmpTree != nullptr) {
                    root_tree->AddRightTree(tmpTree);
                }

                break;
            }

            case dot_tk: {
                if (error_count == 0) {
                    std::cout << "Program was parse successfully" << std::endl;
                    root_tree->PrintTree();
                } else {
                    std::cout << "Got error during parse" << std::endl;
                }

                break;
            }

            default: {
                // XXX: May be like:
                // Here t_iter == eof_tk, if error_flag or error_count == 0
                //   parse was successful
                // else
                //   got error during parse
                break;
            }
        }
    } catch (const std::exception &exp) {
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
int Syntax::vardpParse(Syntax::lex_it &t_iter, Tree *t_tree) {
    // var_list contains list of variables from current code line
    auto var_list = vardParse(t_iter);

    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);

    if (!checkLexem(t_iter, type_tk) && !checkLexem(t_iter, array_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    updateVarTypes(var_list, type_iter->GetName());

    switch (t_iter->GetToken()) {
        case type_tk: {
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

                vardpParse(t_iter, t_tree->GetRightNode());
            } else {
                t_tree->GetRightNode()->FreeRightNode();
            }

            break;
        }

        case array_tk: {


            if (getNextLex(t_iter)->GetToken() != osb_tk) {
                //error - must be osb_tk
            }

            auto valueOfArray = getNextLex(t_iter)->GetName();

            if (getNextLex(t_iter)->GetToken() != dot_tk) {
                //error - must be dot_tk
            } else {
                valueOfArray += t_iter->GetName();
            }

            if (getNextLex(t_iter)->GetToken() != dot_tk) {
                //error - must be dot_tk
            } else {
                valueOfArray += t_iter->GetName();
            }

            valueOfArray += getNextLex(t_iter)->GetName();


            if (getNextLex(t_iter)->GetToken() != of_tk) {
                //errror - must be of_tk
            }

            if (var_list.size() > 1) {
                //error - must be one id for array
            }

            getNextLex(t_iter);
            auto tmptype = getNextLex(t_iter);

            if (tmptype->GetToken() != type_tk) {

            } else {
                //error - must be type of array
            }

            updateVarTypes(var_list, tmptype->GetName());
            updateVarValue(var_list.begin()->c_str(), valueOfArray);

            buildVarTree(var_list, t_tree);

            getNextLex(t_iter);

            auto forwrd_lex = peekLex(1, t_iter);

            if (checkLexem(forwrd_lex, var_tk) || checkLexem(forwrd_lex, id_tk)) {
                if (checkLexem(forwrd_lex, var_tk))
                    getNextLex(t_iter);

                vardpParse(t_iter, t_tree->GetRightNode());
            } else {
                t_tree->GetRightNode()->FreeRightNode();
            }


            break;
        }

        default: {
        }
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
        id_map.emplace(iter->GetName(), Variable("?", "?", false));

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
 * @note Used generating of labels by Pogodin's idea
 */
Tree *Syntax::compoundParse(lex_it &t_iter) {
    static int compound_count = 0; // XXX: How can this be replaced?
    compound_count++;
    int local_lvl = compound_count; // save current compound level
    int sec_prm   = 0;

    auto label = [&]() -> std::string {
        return "_*op" + std::to_string(local_lvl) + "." +
        std::to_string(sec_prm);
    };

    auto is_end = [&]() -> bool {
        return (checkLexem(peekLex(1, t_iter), end_tk)
                || checkLexem(peekLex(1, t_iter), eof_tk));
    };

    Tree *tree               = Tree::CreateNode(t_iter->GetName()); // 'begin'
    auto *root_compound_tree = tree; // save the pointer of start of subtree

    while (t_iter->GetToken() != end_tk) {
        if (t_iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *t_iter);
            return nullptr;
        }

        auto *subTree = stateParse(t_iter, label());

        if (subTree != nullptr) {
            tree->AddRightNode(label());
            (tree->GetRightNode())->AddLeftTree(subTree);
            tree = tree->GetRightNode();

            if (!is_end()) sec_prm++;
        }
    }

    if (checkLexem(peekLex(1, t_iter), dot_tk)
            && checkLexem(peekLex(2, t_iter), eof_tk)) {
        if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
                checkLexem(peekLex(1, t_iter), eof_tk)    ||
                !checkLexem(peekLex(1, t_iter), dot_tk)) {
            printError(MUST_BE_DOT, *t_iter);
            return nullptr;
        }

        tree->AddRightNode(t_iter->GetName() + ".");
    } else if ((checkLexem(peekLex(1, t_iter), unknown_tk)
                || !checkLexem(peekLex(1, t_iter), dot_tk)) &&
               ( checkLexem(peekLex(1, t_iter), eof_tk)
                 || checkLexem(peekLex(2, t_iter), eof_tk))) {
        printError(MUST_BE_DOT, *t_iter);
        return nullptr;
    } else
        tree->AddRightNode(t_iter->GetName());

    return root_compound_tree;
}


/**
 * @brief Parse state part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
Tree* Syntax::stateParse(lex_it &t_iter, const std::string& opValue) {
    Tree *result_tree = nullptr;
    auto iter = getNextLex(t_iter);
    static std::string valueTreeForBreakTree;

    switch (iter->GetToken()) {
        case id_tk: {
            if (!isVarExist(iter->GetName())) {
                printError(UNKNOWN_ID, *t_iter);
                return nullptr;
            }

            auto var_iter = iter;
            getNextLex(t_iter);

            if (!checkLexem(t_iter, ass_tk) && !checkLexem(t_iter, osb_tk)) {
                printError(MUST_BE_ASS, *t_iter);
                return nullptr;
            }

            auto* tree_exp = new Tree;

            switch (t_iter->GetToken()) {
                case ass_tk: {
                    tree_exp = Tree::CreateNode(t_iter->GetName());
                    tree_exp->AddLeftNode(var_iter->GetName());

                    expressionParse(t_iter, tree_exp);

                    break;
                }

                case osb_tk: {
                    tree_exp = Tree::CreateNode("$");
                    tree_exp->AddLeftNode(var_iter->GetName());

                    auto* tree_osbTMP = new Tree;
                    tree_osbTMP->ChangeValue(":=");

                    expressionParse(t_iter, tree_osbTMP);

                    tree_exp->AddRightTree(tree_osbTMP->GetRightNode());


                    auto* tmpTreeForResult = new Tree;
                    getNextLex(t_iter);
                    tmpTreeForResult = Tree::CreateNode(t_iter->GetName());
                    tmpTreeForResult->AddLeftTree(tree_exp);

                    expressionParse(t_iter, tmpTreeForResult);
                    tree_exp = tmpTreeForResult;
                    break;
                }

                default:
                    break;
            }

            if (!checkLexem(t_iter, semi_tk)) { // we exit from expression on the ';'
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }


            //tree_exp->PrintTree();
            result_tree = tree_exp;
            break;
        }

        case begin_tk: {
            auto *tree_comp = compoundParse(t_iter);
            getNextLex(t_iter);

            if (!checkLexem(t_iter, semi_tk)) {
                printError(MUST_BE_SEMI, *t_iter);
                return nullptr;
            }

            if (tree_comp != nullptr)
                result_tree = tree_comp;

            break;
        }

        case if_tk: {
            Tree* tmpTree = new Tree();
            Tree* if_tree = new Tree();
            tmpTree->ChangeValue(":=");

            expressionParse(t_iter, tmpTree);
            if_tree->AddLeftTree(tmpTree->GetRightNode());

            if_tree->ChangeValue("if");

            if (t_iter->GetToken() != then_tk) {
                //error with if then
            }

            if_tree->AddRightNode("then");
            tmpTree = if_tree->GetRightNode();
            (if_tree->GetRightNode())->AddLeftTree(stateParse(t_iter, ""));

            if (t_iter->GetToken() != semi_tk) {
                //error with ;
            }

            getNextLex(t_iter);

            if (t_iter->GetToken() == else_tk)
                (if_tree->GetRightNode())->AddRightTree(stateParse(t_iter, ""));

            if (t_iter->GetToken() != semi_tk) {
                //error with ;
            }

            if (if_tree != nullptr)
                result_tree = if_tree;

            break;
        }

        case for_tk: {
            valueTreeForBreakTree = opValue;
            Tree* for_tree = new Tree();
            Tree* tmpTree = new Tree();

            tmpTree->ChangeValue(":=");
            for_tree->ChangeValue("for");

            auto iter = getNextLex(t_iter);

            tmpTree->AddLeftNode(t_iter->GetName());
            t_iter = getNextLex(t_iter);
            expressionParse(t_iter, tmpTree);

            if (t_iter->GetToken() == to_tk) {
                for_tree->AddLeftNode("<=");
                (for_tree->GetLeftNode())->AddLeftTree(tmpTree);
                expressionParse(t_iter, for_tree->GetLeftNode());
            } else if (t_iter->GetToken() == downto_tk) {
                for_tree->AddLeftNode(">=");
                (for_tree->GetLeftNode())->AddLeftTree(tmpTree);
                expressionParse(t_iter, for_tree->GetLeftNode());
            } else {
                //error
            }

            if (t_iter->GetToken() == do_tk) {
                for_tree->AddRightTree(stateParse(t_iter, ""));
            } else {
                //error
            }

            if (for_tree != nullptr)
                result_tree = for_tree;

            break;
        }

        case while_tk: {
            valueTreeForBreakTree = opValue;
            Tree* tmpTree = new Tree();
            Tree* while_tree = new Tree();
            tmpTree->ChangeValue(":=");

            expressionParse(t_iter, tmpTree);
            while_tree->AddLeftTree(tmpTree->GetRightNode());

            while_tree->ChangeValue("while");

            if (t_iter->GetToken() != do_tk) {
                //error with if then
            }

            while_tree->AddRightTree(stateParse(t_iter, ""));

            if (while_tree != nullptr)
                result_tree = while_tree;

            break;
        }

        case break_tk: {
            Tree* break_tree = Tree::CreateNode(t_iter->GetName());
            break_tree->AddRightNode(valueTreeForBreakTree);

            if (break_tree != nullptr)
                result_tree = break_tree;

            break;
        }

        default: {
            break;
        }
    }

    return result_tree;
}


/**
 * @brief Parse expression part
 * @param[inout] t_iter - iterator of table of lexeme
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */
int Syntax::expressionParse(lex_it &t_iter, Tree *tree) {
    lex_it var_iter;
    Tree *subTree;

    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case id_tk: {
            if (!isVarExist(iter->GetName()))
                printError(UNKNOWN_ID, *t_iter);
        }

        case negnum_tk:
        case constant_tk: {
            var_iter = iter; // save variable/constant value
            subTree  = simplExprParse(var_iter, t_iter, tree);
            break;
        }

        case opb_tk: {
            auto tmpIter = iter;
            auto* tmptreeExp = new Tree;
            tmptreeExp->ChangeValue(":=");
            expressionParse(t_iter, tmptreeExp);
            auto* tmptree = new Tree;
            tmptree->ChangeValue(":=");
            //TODO: check nullptr
            simplExprParseForTree(tmptreeExp->GetRightNode(), t_iter, tmptree);
            tree->AddRightTree(tmptree->GetRightNode());
            break;
        }

        default: {
            printError(MUST_BE_ID, *t_iter);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse subexpression part
 * @param[in]    var_iter - iterator, which point to the variable (id/number)
 * @param[inout] t_iter   - iterator of table of lexeme
 * @param[inout] tree     - current subtree
 *
 * @return subtree of subexpression
 */
Tree* Syntax::simplExprParse(Syntax::lex_it& var_iter,
                             Syntax::lex_it& t_iter, Tree* tree) {
    Tree* subTree;
    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case add_tk:            // TODO: And check the types of variables
        case sub_tk:            //   during operations
        case mul_tk:
        case xor_tk:
        case and_tk:
        case or_tk:
        case div_tk:
        case lequ_tk:
        case mequ_tk:
        case less_tk:
        case more_tk:
        case eqv_tk:
        case newton_tk: {
            if (operations.at(iter->GetName()) <=
                    operations.at(tree->GetValue())) {       // Priority of current <=
                tree->AddRightNode(var_iter->GetName());
                subTree = tree->GetParentNode();

                while (operations.at(iter->GetName()) <= // go through parents
                        operations.at(subTree->GetValue()))
                    subTree = subTree->GetParentNode();

                subTree = createLowestOpTree(subTree, iter->GetName());
            } else { // if Priority of current >
                /******* Create a new node of subexpression ************/
                tree->AddRightNode(iter->GetName());            //     <oper> <- subTree
                subTree = tree->GetRightNode();                 //      /  /
                subTree->AddLeftNode(var_iter->GetName());      //    val  nullptr
                /********************************************************/
            }

            expressionParse(t_iter, subTree);
            break;
        }

        default: { // any other lexem, expression is over
            tree->AddRightNode(var_iter->GetName());
            break;
        }
    }

    return tree;
}

Tree* Syntax::simplExprParseForTree(Tree* treeCPB, lex_it& t_iter, Tree* tree) {
    Tree* subTree;
    auto iter = getNextLex(t_iter);

    switch (iter->GetToken()) {
        case add_tk:            // TODO: And check the types of variables
        case sub_tk:            //   during operations
        case mul_tk:
        case xor_tk:
        case and_tk:
        case or_tk:
        case div_tk:
        case lequ_tk:
        case mequ_tk:
        case less_tk:
        case more_tk:
        case eqv_tk:
        case newton_tk: {
            if (operations.at(iter->GetName()) <=
                    operations.at(tree->GetValue())) {       // Priority of current <=
                tree->AddRightTree(treeCPB);
                subTree = tree->GetParentNode();

                while (operations.at(iter->GetName()) <= // go through parents
                        operations.at(subTree->GetValue()))
                    subTree = subTree->GetParentNode();

                subTree = createLowestOpTree(subTree, iter->GetName());
            } else { // if Priority of current >
                /******* Create a new node of subexpression ************/
                tree->AddRightNode(iter->GetName());            //     <oper> <- subTree
                subTree = tree->GetRightNode();                 //      /  /
                subTree->AddLeftTree(treeCPB);      //    val  nullptr
                /********************************************************/
            }

            expressionParse(t_iter, subTree);
            break;
        }

        default: { // any other lexem, expression is over
            tree->AddRightTree(treeCPB);;
            break;
        }
    }

    return tree;
}

Tree* Syntax::boolExprParse(lex_it& t_iter, Tree* tree) {
    Tree* tmpTree = new Tree();
    tmpTree->ChangeValue(":=");
    expressionParse(t_iter, tmpTree);
    tree->AddLeftTree(tmpTree->GetRightNode());

    switch (t_iter->GetToken()) {
        case lequ_tk:
        case mequ_tk:
        case less_tk:
        case more_tk:
        case eqv_tk:
        case newton_tk: {
            tree->ChangeValue(t_iter->GetName());
            getNextLex(t_iter);
            expressionParse(t_iter, tmpTree);
            tree = tmpTree;
            return tree;
        }
    }

    return nullptr;
}



/**
 * @brief Print information about error
 * @param[in] t_err - error type
 * @param[in] lex   - error lexeme
 *
 * @return none
 */
void Syntax::printError(errors t_err, Lexem lex) {
    error_count++;

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
                      << "' on " << lex.GetLine() << " line"       << std::endl;
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
                      << "' on " << lex.GetLine() << " line"  << std::endl;
            break;
        }

        case UNKNOWN_ID: {
            std::cerr << "<E> Syntax: Undefined variable '"  << lex.GetName()
                      << "' on " << lex.GetLine() << " line" << std::endl;
            break;
        }

        case MUST_BE_DOT: {
            std::cerr << "<E> Syntax: Program must be end by '.'" << std::endl;
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

            iter++;
            N--;
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
    if (t_iter == lex_table.end())
        return false;

    if (t_iter->GetToken() != t_tok)
        return false;

    return true;
}


/**
 * @brief Check existence of variable
 * @param[in] t_var_name - variable for check
 *
 * @return true  - if variable is exist
 * @return false - if unknown variable (doesn't exist)
 */
bool Syntax::isVarExist(const std::string &t_var_name) {
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
        for (auto &el : t_var_list)
            id_map.at(el).type = t_type_name;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax::updateVarValue(const std::string& name, const std::string& value) {
    id_map.at(name).value = value;
    id_map.at(name).flagArray = true;
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
void Syntax::buildVarTree(const std::list<std::string> &t_var_list, Tree *t_tree) {
    try {
        auto i = 0;

        for (auto &el : t_var_list) {
            auto *tmp_tree  = Tree::CreateNode(el);
            tmp_tree->AddRightNode(id_map.at(el).type);

            if (id_map.at(el).flagArray == true)
                if (id_map.at(el).value != "?") {
                    tmp_tree->AddLeftNode("array");
                    tmp_tree->GetLeftNode()->AddLeftNode(id_map.at(el).value);
                }

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
 *
 * How look t_donor_tree:
 *                  a           <id>
 *                   \             \
 *                   integer       <type>
 */
void Syntax::createVarTree(Tree *t_tree, Tree *t_donor_tree, int lvl) {
    if (lvl > 0) {
        lvl--;
        createVarTree(t_tree->GetRightNode(), t_donor_tree, lvl);
    } else {
        t_tree->AddLeftTree(t_donor_tree);
        t_tree->AddRightNode("$");
    }
}


/**
 * @brief Create subtree with lowest operator priority
 * @param[in] t_parent_tree - pointer to parent tree of subtree with lowest operator
 * @param[in] value         - value of the lowest subtree
 *
 * @return pointer to subtree with the lowest operator
 * @note We find in main tree the subtree with equal operator (between value)
 *   and set this subtree like the parent tree (t_parent_tree) for new
 *   lowest subtree.
 *   All children nodes of t_parent_tree will be set like children nodes of
 *   the lowest operator subtree
 */
Tree* Syntax::createLowestOpTree(Tree *t_parent_tree, std::string value) {
    auto *lowest_tree = Tree::CreateNode(t_parent_tree, value);
    lowest_tree->AddLeftTree(t_parent_tree->GetRightNode());
    t_parent_tree->AddRightTree(lowest_tree);

    return lowest_tree;
}