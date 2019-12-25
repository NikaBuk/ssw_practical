# task_04
Simple example of lexical and syntax analyzer. With building AST (*abstract syntax tree*) and code generation GAS (*GNU Assembler*).


### What you have to do:
- add code generation;
- add all previous tasks;
- make error informator like a separate entity (class);
- implement all 'TODO' tips (start with `TODO: ...`);
- (optional, +3) add initialization of value by expressions;
- (optional, +2) add optimization;
- (optional, +1) add script for compilation assembler's code;
- (optional, +1) implement constant (for tree and GAS).


### Build
*Note:* all instructions here for machine with *nix OS. For Windows users may be need some another instructions (you can try to use `Cygwin`/`MinGW` or just change `g++` on `mingw`).

To build by make (Makefile):
```
$ cd <to repo path>/task_04
$ make
```

To build by cmake (CMakeLists.txt):
```
$ cd <to repo path>/task_04
$ cmake .
$ make
```

# task_04
Пример лексического и синтаксического анализатора. С построением АСД (*абстрактно синтаксического дерева*) и генерацией кода GAS (*GNU Assembler*).

### Что вы должны сделать:
- добавить генерацию кода (по сгенерированному синтаксическому дереву);
- добавить решения всех предыдущих задач;
- вынести вывод ошибок как отдельную сущность (класс);
- реализовать все подсказки 'TODO' (начинаются с `TODO: ...`);
- (необязательно, +3) добавить инициализацию переменных посредством выражений;
- (необязательно, +2) добавить оптимизацию;
- (необязательно, +1) добавить скрипт для компиляции кода ассемблера;
- (необязательно, +1) реализовать работу с константами (для дерева и GAS).


### Имейте в виду:
Данная работа должна являться (по сути) продолжением заданий `task_01`, `task_02`, `task_03`. Т.е.
 в данной работе у вас должны быть выполнены все прошлые задания (к примеру,
 реализация работы со скобками, построение дерева для входной программы, etc).
Соответственно, в местах где отсутствует реализация предыдущих заданий, должна быть
ваша часть (вне зависимости от наличия/отсутствия меток `TODO`).

Так же, по возможности, прошу всех перед коммитом и отправкой правок (*push*)
воспользоваться утилитой нормализации кода [Artistic Style](http://astyle.sourceforge.net/).
Скачать её можно по [этой](https://sourceforge.net/projects/astyle/files/latest/download) ссылке.
Для использования утилиты, можете воспользоваться скриптом *AStyle.bat*.
Через консоль произведите:
```
cd <к папке с task_04>
AStyle <путь до AStyle\bin> <путь до \task_04\src>
```
После исполнения данной утилиты у вас появяться отформатированные *.cpp/.h* файлы
и их оригиналы (*.cpp.orig*/*.h.orig*).


### Отчёт
В отчёте к заданиям должно содержаться:
- перечисление **выполненых** заданий;
- листинг (код) ключевых изменений (2-4 примера, решение основных задач);
- вывод скомпилированной программы (с ошибками и без);
- вывод сгенерированного ассемблерного кода;
- вывод по проделанной работе.
