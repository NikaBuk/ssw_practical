cmake_minimum_required(VERSION 3.13)
project(lecs_pars)


set(CMAKE_CXX_STANDARD 14)
set(SRC ./src)

add_executable(lecs_pars ${SRC}/main.cpp ${SRC}/task_04.h ${SRC}/task_04.cpp
               ${SRC}/Lexer.cpp ${SRC}/Lexer.h ${SRC}/Syntax.cpp ${SRC}/Syntax.h
               ${SRC}/Lexem.cpp ${SRC}/Lexem.h ${SRC}/Variable.cpp
               ${SRC}/Variable.h ${SRC}/Tree.cpp ${SRC}/Tree.h
               ${SRC}/GenCode.cpp ${SRC}/GenCode.h)
