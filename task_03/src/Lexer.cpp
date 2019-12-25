//
// Created by vovan on 28.09.2019.
//

#include "Lexer.h"


Lexer::Lexer(const char *file_path) {
    try {
        code.open(file_path);
    } catch (const std::exception &exp) {
        std::string what;
        std::string lel(exp.what());
        what = "<E> Lexer: Catch exception in constructor: ";

        throw std::runtime_error(what + lel);
    }
}


Lexer::~Lexer() {
    code.close();
}


/**
 * @brief Scan code and build table of lexemes
 * @param[in] file_path - path to the file with code
 *
 * @return table of lexeme
 */
std::vector<Lexem> Lexer::ScanCode() {
    try {
        if (!code.is_open()) {
            std::cerr << "<E> Can't open file" << std::endl;
            return lex_table;
        }

        while (!code.eof()) {
            lex_table.emplace_back(getLex());
        }

        return lex_table;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what() <<
                  std::endl;
        return lex_table;
    }
}


void Lexer::incI() {
    i++;
}

/**
 * @brief Get next lexeme from input filestream
 * @param none
 *
 * @return lexeme
 */
Lexem Lexer::getLex() {
    try {

        i++;
        auto ch = getCurrentCurs();

        while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            if (ch == '\n') line++;

            if (code.eof())
                return Lexem("EOF", eof_tk, line);

            ch = getChar();
        }

        auto isId = [](char ch) {
            return std::isalpha(static_cast<unsigned char>(ch)) ||
                   std::isdigit(static_cast<unsigned char>(ch));
        };


        std::string lex;

        if (std::isdigit(static_cast<unsigned char>(ch))) { // Constants (Numbers)
            while (std::isdigit(static_cast<unsigned char>(ch))) {
                lex += ch;
                ch = getChar();
            }


            return Lexem(std::move(lex), constant_tk, line);
        } else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
            while (isId(ch)) {
                lex += ch;
                ch = getChar();
            }

            if (lex == "program")      {
                return Lexem(std::move(lex), program_tk, line);
            } else if (lex == "var")     {
                return Lexem(std::move(lex), var_tk, line);
            } else if (lex == "begin")   {
                return Lexem(std::move(lex), begin_tk, line);
            } else if (lex == "integer") {
                return Lexem(std::move(lex), type_tk, line);
            } else if (lex == "boolean") {
                return Lexem(std::move(lex), type_tk, line);
            } else if (lex == "end")     {
                return Lexem(std::move(lex), end_tk, line);
            } else if (lex == "div")     {
                return Lexem(std::move(lex), div_tk, line);
            } else if (lex == "xor")     {
                return Lexem(std::move(lex), xor_tk, line);
            } else if (lex == "or")      {
                return Lexem(std::move(lex), or_tk, line);
            } else if (lex == "and")     {
                return Lexem(std::move(lex), and_tk, line);
            } else if (lex == "if")      {
                return Lexem(std::move(lex), if_tk, line);
            } else if (lex == "for")     {
                return Lexem(std::move(lex), for_tk, line);
            } else if (lex == "else")    {
                return Lexem(std::move(lex), else_tk, line);
            } else if (lex == "then")    {
                return Lexem(std::move(lex), then_tk, line);
            } else if (lex == "to")      {
                return Lexem(std::move(lex), to_tk, line);
            } else if (lex == "downto")  {
                return Lexem(std::move(lex), downto_tk, line);
            } else if (lex == "do")      {
                return Lexem(std::move(lex), do_tk, line);
            } else if (lex == "of") {
                return Lexem(std::move(lex), of_tk, line);
            } else if (lex == "array") {
                return Lexem(std::move(lex), array_tk, line);
            } else if (lex == "while") {
                return Lexem(std::move(lex), while_tk, line);
            } else if (lex == "break")   {
                return Lexem(std::move(lex), break_tk, line);
            } else { // it is ID
                return Lexem(std::move(lex), id_tk, line);
            }
        } else if (std::ispunct(static_cast<unsigned char>(ch))) { // Other symbols
            tokens tok;

            switch (ch) {
            case ',' :
                tok = comma_tk;
                break;

            case '[':
                tok = osb_tk;
                break;

            case ']':
                tok = csb_tk;
                break;

            case '.' :
                tok = dot_tk;
                break;

            case ':' :
                tok = ddt_tk;
                break;

            case ';' :
                tok = semi_tk;
                break;

            case '=' :
                tok = eqv_tk;
                break;

            case '(' :
                tok = opb_tk;
                break;

            case ')' :
                tok = cpb_tk;
                break;

            case '*' :
                tok = mul_tk;
                break;

            case '+' :
                tok = add_tk;
                break;

            case '-': {
                tok = sub_tk;

                break;
            }

            case '>': {
                lex = ch;
                std::string tmpLex{ peekChar(0) };

                if (tmpLex == "=") {
                    lex += tmpLex;
                    getChar();
                    getChar();
                    return Lexem(std::move(lex), mequ_tk, line);
                } else {
                    tok = more_tk;
                    getChar(); // some kind of k o s t y l; here we look on \n
                    return Lexem(std::move(lex), tok, line);
                }

                break;
            }

            case '<': {
                lex = ch;
                std::string tmpLex{ peekChar(0) };

                if (tmpLex == "=") {
                    lex += tmpLex;
                    getChar();
                    getChar();
                    return Lexem(std::move(lex), lequ_tk, line);
                } else if (tmpLex == ">") {
                    lex += tmpLex;
                    getChar();
                    getChar();
                    return Lexem(std::move(lex), newton_tk, line);
                } else {
                    tok = less_tk;
                    getChar(); // some kind of k o s t y l; here we look on \n
                    return Lexem(std::move(lex), tok, line);
                }

                break;
            }

            default: {
                std::cerr << "<E> Unknown token " << ch << std::endl;
                tok = unknown_tk;
                break;
            }

            }

            lex += ch;

            if (tok == ddt_tk) {
                ch = getChar();

                if (ch == '=') {
                    lex += ch;
                    tok = ass_tk;
                }
            }

            getChar(); // some kind of k o s t y l; here we look on \n
            return Lexem(std::move(lex), tok, line);
        } else {
            std::cerr << "<E> Unknown token " << ch << std::endl;
        }

        return Lexem("", unknown_tk, line);
    } catch (const std::exception &exp) {
        return Lexem("", unknown_tk, line);
    }
}


/**
 * @brief Get next character from input filestream
 * @param none
 *
 * @return next character
 */
char Lexer::getChar() {
    if (code.fail()) {
        std::cerr << "<E> Can't read from the file" << std::endl;
        throw std::runtime_error("File doesn't available");
    }

    if (!code.eof()) {
        code >> std::noskipws >> cursor;
    } else {
        std::cerr << "<E> File is EOF early" << std::endl;
        throw std::runtime_error("File is EOF early");
    }

    return cursor;
}

char Lexer::peekChar(int n) {
    try {
        char ch = -1;
        int curr_pos = code.tellg(); // get current position in stream

        code.seekg(curr_pos + n, code.beg); // set needed position in stream
        code >> std::noskipws >> ch;    // get character from stream with ' '
        code.seekg(curr_pos, code.beg); // return previous position in stream

        return ch;
    } catch (std::exception & exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what()
                  << std::endl;
        return -1;
    }
}

Lexem Lexer::peekLex(int n) {
    int curr_pos = code.tellg(); // get current position in stream
    auto curr_curs = getCurrentCurs();

    try {
        Lexem res;

        for (int i = 0; i < n; i++) {
            res = getLex();
        }

        code.seekg(curr_pos, code.beg);
        cursor = curr_curs;

        return res;
    } catch (const std::exception & exp) {
        std::cerr << "<E> You try to peek too much forward, get back" << std::endl;
        code.seekg(curr_pos, code.beg);
        cursor = curr_curs;
    }
}
