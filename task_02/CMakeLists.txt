
project(lecs_pars)

set(SRC ./src)

add_executable(lecs_pars ${SRC}/main.cpp ${SRC}/task_02.h ${SRC}/task_02.cpp
               ${SRC}/Lexer.cpp ${SRC}/Lexer.h ${SRC}/Syntax.cpp ${SRC}/Syntax.h
               ${SRC}/Lexem.cpp ${SRC}/Lexem.h ${SRC}/Variable.cpp
               ${SRC}/Variable.h )
