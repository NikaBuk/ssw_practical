//
// Created by vovan on 28.09.2019.
//

#ifndef LECS_PARS_LEXEM_H
#define LECS_PARS_LEXEM_H

#include <string>


enum tokens {
    unknown_tk = -1,    // we get unknown token
    program_tk = 0,     // 'program'
    var_tk,             // 'var'
    begin_tk,           // 'begin'
    end_tk,             // 'end'
    type_tk,            // 'type'
    id_tk = 8,          // any [aA-zZ][0-9]
    constant_tk = 9,    // 0-9
    dot_tk,             // '.'
    comma_tk,           // ','
    ddt_tk,             // ':'
    semi_tk,            // ';'
    eqv_tk,             // '='
    ass_tk,             // ':='
    add_tk,             // '+'
    sub_tk,             // '-'
    negnum_tk,          // '-[0-9]'
    mul_tk,             // '*'
    div_tk,             // '/'
    opb_tk,             // '('
    cpb_tk,             // ')'
    xor_tk,             // 'xor'
    or_tk,              // 'or'
    and_tk,             // 'and'
    not_tk,             // 'not'
    less_tk,            // '<'
    more_tk,            // '>'
    lequ_tk,            // '<='
    mequ_tk,            // '>='
    newton_tk,          // '<>'
    if_tk,              // 'if'
    for_tk,             // 'for'
    array_tk,           // 'array'
    then_tk,            // 'then'
    else_tk,            // 'else'
    to_tk,              // 'to'
    do_tk,              // 'do'
    downto_tk,          // 'downto'
    while_tk,           // 'while'
    of_tk,              // 'of'
    osb_tk,             // '['
    csb_tk,             // ']'
    break_tk,           // 'break'
    bracel_tk,          // '{'
    bracer_tk,          // '}'

    eof_tk              // end of file
};


enum errors {
    UNKNOWN_LEXEM,   // Unknown lexeme
    EOF_ERR,         // End of file error
    MUST_BE_ID,      // Error: Must be identifier
    MUST_BE_SEMI,    // Error: Must be ';'
    MUST_BE_PROG,    // Error: Must be 'program'
    MUST_BE_COMMA,   // Error: Must be ','
    MUST_BE_TYPE,    // Error: Must be type of identifier
    MUST_BE_DOT,     // Error: Must be '.'
    MUST_BE_ASS,     // Error: Must be ':='
    DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
    UNKNOWN_ID,      // Error: Undefined identifier
    MUST_BE_OSB,  // ERROR: MUST BE '['
    INVALID_RANGE_OF_ARRAY, // ERROR: INVALID RANGE LIKE 3..2
    MUST_BE_CSB,      //ERROR: MUST BE ']'
    MUST_BE_OF,       //ERROR: MUST BE 'OF'
    MUST_BE_ONE_ID_ARRAY,   //ERROR: MUST BE ONE ID OF ARRAY
    WRONG_TYPE_MUST_BE_INTEGER, //ERROR: ERROR TYPE MUST BE INTEGER
    MUST_BE_THEN,           //ERROR: MUST BE 'THEN'
    MUST_BE_OPERATOR_GE_OR_LE,  //ERROR: MUST BE '<=' OR '>='
    MUST_BE_CONSTANT,       //ERROR: MYST BE CONSTANT
    WRONG_INIT,             //WROND INIT ARRAY - invalid array initialization
    MUST_BE_BRACER,         //MUST BE BRACER
    MUST_BE_CPB,         //MUST BE ')'
    WRONG_USE_ARRAY,     //MUST BE ID INSTEAD ARRAY
    WRONG_USE_ID,        //MUST BE ARRAY INSTEAD ID
    MUST_BE_DO          //ERROR: MUST BE 'DO'
};


class Lexem {
public:
    Lexem() : name(""), token(unknown_tk), line(0) {};
    Lexem(std::string &&t_name, tokens t_tok, int t_line) : name(t_name),
        token(t_tok), line(t_line) {};

    std::string GetName();
    tokens      GetToken();
    int         GetLine();
private:
    std::string name;
    tokens      token;
    int         line;
};


#endif //LECS_PARS_LEXEM_H
