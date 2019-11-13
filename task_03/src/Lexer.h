//
// Created by vovan on 28.09.2019.
//

#ifndef LECS_PARS_LEXER_H
#define LECS_PARS_LEXER_H


#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>
#include "Lexem.h"


class Lexer {
public:
    explicit Lexer(const char *file_path);
    std::vector<Lexem> ScanCode();

    ~Lexer();
private:
    std::ifstream      code;
    char               cursor  { -1 };
    int                line    { 1 };
    std::vector<Lexem> lex_table;
    int i{ 0 };

    void incI();
    Lexem getLex();
    char  getChar();
    char  peekChar(int n);
    Lexem peekLex(int n);

    inline char getCurrentCurs() {
        return cursor;
    }
};


#endif //LECS_PARS_LEXER_H
