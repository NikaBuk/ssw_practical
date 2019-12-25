#include "task_03.h"


/**
 *
 * program name;
 * var a : integer;
 *     b : integer;
 *     c : array[1..5] of integer;
 * begin
 * b := 1;
 * a := 2;
 * end.
 *
 *
 *             table of lexem:
 *  <lexem_str>, <lexem_id>, <line in code>
 */


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


int Parse3(const std::string& file_path) {
    Lexer lex(file_path.c_str());
    auto table = lex.ScanCode();

    if(table[0].GetToken() == eof_tk) {
        std::cout<<"There is a one token - EOF"<<std::endl;
        return -1;
    }

    Syntax syntx(std::move(table));
    syntx.ParseCode();

    return EXIT_SUCCESS;
}
