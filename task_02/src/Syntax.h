//
// Created by vovan on 28.09.2019.
//

#ifndef LECS_PARS_SYNTAX_H
#define LECS_PARS_SYNTAX_H

#include <algorithm>
#include <iostream>
#include <chrono>
#include <list>
#include <map>
#include <vector>
#include "Lexem.h"
#include "Variable.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include "task_02.h"
#include "SynAri.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include "tree_t.h"

struct st
{
	char c; struct st* next;
};
/* Функция push записывает на стек (на веpшину котоpого указывает HEAD)
   символ a . Возвpащает указатель на новую веpшину стека */
struct st* push(struct st* HEAD, char a)
{
	struct st* PTR;
	/* Выделение памяти */
	if ((PTR = new st) == NULL)
	{
		/* Если её нет - выход */
		puts("Нет памяти"); exit(-1);
	}
	/* Инициализация созданной веpшины */
	PTR->c = a;
	/* и подключение её к стеку */
	PTR->next = HEAD;
	/* PTR -новая веpшина стека */
	return PTR;
}

/* Функция DEL удаляет символ с веpшины стека.
   Возвpащает удаляемый символ.
   Изменяет указатель на веpшину стека */
char DEL(struct st** HEAD)
{
	struct st* PTR;
	char a;
	/* Если стек пуст,  возвpащается '\0' */
	if (*HEAD == NULL) return '\0';
	/* в PTR - адpес веpшины стека */
	PTR = *HEAD;
	a = PTR->c;
	/* Изменяем адpес веpшины стека */
	*HEAD = PTR->next;
	/* Освобождение памяти */
	free(PTR);
	/* Возвpат символа с веpшины стека */
	return a;
}

/* Функция PRIOR возвpащает пpиоpитет аpифм. опеpации */
int PRIOR(char a)
{
	switch (a)
	{
	case '*':
	case '/':
		return 3;

	case '-':
	case '+':
		return 2;

	case '(':
		return 1;
	}
}




class Syntax {
public:
	explicit Syntax(std::vector<Lexem>&& t_lex_table);
	int ParseCode();
	~Syntax();
	tree_t getPascal_tree();
private:
	using lex_it = std::vector<Lexem>::iterator; // alias of vector iterator
	lex_it                          cursor;
	std::vector<Lexem>              lex_table;   // out table of lexemes
	std::map<std::string, Variable> id_map;      // our table of identifiers
	tree_t* pascal_tree; // our syntax tree

	lex_it getNextLex(lex_it& iter);
	lex_it peekLex(int N, lex_it t_iter);

	tree_t* buildTreeStub(tree_t* t_tree, const std::string& node_name);
	tree_t* createNode(const std::string& node_name);

	std::pair<int,tree_t*>  expressionParse(lex_it& t_iter, std::string tmp = "");
	int   stateParse(lex_it& t_iter);
	int   compoundParse(lex_it& t_iter);
	int   vardpParse(lex_it& t_iter, tree_t* t_tree);
	std::list<std::string> vardParse(lex_it& t_iter);
	int   blockParse(lex_it& t_iter);
	int   programParse(lex_it& t_iter);

	void printError(errors t_err, Lexem lex);
	bool checkLexem(const lex_it& t_iter, const tokens& t_tok);
	bool isVarExist(const std::string& t_var_name);
	void updateVarTypes(const std::list<std::string>& t_var_list,
		const std::string& t_type_name);
	void buildVarTree(const std::list<std::string>& t_var_list, tree_t* t_tree);

	void createVarTree(tree_t* t_tree, tree_t* t_donor_tree, int lvl);
	void freeTreeNode(tree_t*& t_tree);
};


#endif //LECS_PARS_SYNTAX_H
