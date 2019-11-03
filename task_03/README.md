# task_03
Simple example of lexical and syntax analyzer. With building AST (*abstract syntax tree*).
Work with if/while/for operators.

### What you have to do:
- add operators of if/while/for;
- add logical/bitwise operators of or/and/xor/nand;
- add new types: Boolean;
- add arrays;
- implement all 'TODO' tips (start with `TODO: ...`);
- (optional, +1) implement serialization of tree;



### Build
*Note:* all instructions here for machine with *nix OS. For Windows users may be need some another instructions (you can try to use `Cygwin`/`MinGW` or just change `g++` on `mingw`).

To build by make (Makefile):
```
$ cd <to repo path>/task_03
$ make
```

To build by cmake (CMakeLists.txt):
```
$ cd <to repo path>/task_03
$ cmake .
$ make
```

# task_03
Пример лексического и синтаксического анализатора. С построением АСД (*абстрактно синтаксического дерева*).

### Что вы должны сделать:
- добавить операторы if/while/for (и соответственно, построение деревьев для них);
- добавить логические/побитовые операции оr/and/xor/nand;
- добавить типы данных: Boolean;
- добавить массивы;
- реализовать все 'TODO' метки (начинаются с `TODO: ...`);
- (опционально, +1) добавить сериализацию дерева;


### Имейте в виду:
Данная работа должна являться (по сути) продолжением задания `task_02`. Т.е.
 в данной работе у вас должны быть выполнены все прошлые задания (к примеру,
 реализация работы со скобками, построение дерева для входной программы, etc).
Соответственно, в местах где отсутствует реализация второго задания, должна быть
ваша часть (вне зависимости от наличия/отсутствия меток `TODO`).

Так же, по возможности, прошу всех перед коммитом и отправкой правок (*push*)
воспользоваться утилитой нормализации кода [Artistic Style](http://astyle.sourceforge.net/).
Скачать её можно по [этой](https://sourceforge.net/projects/astyle/files/latest/download) ссылке.
Для использования утилиты, можете воспользоваться скриптом *AStyle.bat*.
Через консоль произведите:
```
cd <к папке с task_03>
AStyle <путь до AStyle\bin> <путь до \task_03\src>
```
После исполнения данной утилиты у вас появяться отформатированные *.cpp/.h* файлы
и их оригиналы (*.cpp.orig*/*.h.orig*).


### Отчёт
В отчёте к заданиям (*task_01*, *task_02*, *task_03*, etc) должно содержаться:
- перечисление **выполненых** заданий;
- листинг (код) ключевых изменений (2-4 примера, решение основных задач);
- вывод скомпилированной программы;
- вывод по проделанной работе.
