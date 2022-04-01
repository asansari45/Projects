# Overview
The InterpreterProject is an interpreter having a simple grammar.  The implementation of the interpreter is done using the freely-available [Visual Studio Community C++ Version(https://visualstudio.microsoft.com/vs/community/). The parser is constructed using [GNU Bison](https://www.gnu.org/software/bison/).  The parser produces a set of nodes that represent a linked-list of binary trees.  The nodes are then evaluated using the popular Visitor design pattern.  The interpreter can be run in a file mode or interactive mode.

## Language

The language of the interpreter consists of the following major features:

1.  Expressions with C-Style Operators
    - +,-,/,*,
    - <,>,>=,<=
    - !=, ==
    - &&, ||
    - ^, |, &,
    - ~
1.  Atomic Types:  CHAR, UINT, INT, FLT, and STR.
1.  Arrays
1.  Variables:  Global and Local Scope
1.  Literals:  Binary, Decimal, and Hexadecimal
1.  Built-in Functions:  srand(), rand(), load(), print(), help().
1.  Interpreter Functions:  .vars, .functions, .quit.
1.  Functions with Parameters
1.  Reference Parameters

### Expressions with C-Style Operators

The expressions should be very self-explanatory.  Here are examples in interactive mode:

```
>8+9
=17>90*8-9/9+4
=723>

>0xf | 0xf0
=255U>

and so on and so forth...
```

### Atomic Types:  CHAR, UINT, INT, FLT, and STR
The interpreter supports the C-Style: char, unsigned int, int, float, and std::string types.  An example from the interpreter is shown here:

```
>a='a'
>b=0x89
>c=5
>d=5.5
>e="My String"
>.vars
Symbol Table Name:  GLOBAL
NAME                  TYPE   DIMS          VALUE(S)
===================================================
a                     CHAR   ---           a
b                     UINT   ---           137U
c                     INT    ---           5
d                     FLT    ---           5.5
e                     STR    ---           My String
main                  INT    ---           0
```

### Arrays
The interpreter also supports arrays of all the Atomic Types from the previous section.  The array variables may be 1, 2, or 3 dimensions.  When an array is created, by default it starts out as an array of INTs.  It transforms itself to other types as elements are written to the array.  Examples are shown here:

```
>a=dim[10]
>b=dim[10,10]
>c=dim[10,10,10]
>.vars
Symbol Table Name:  GLOBAL
NAME                  TYPE   DIMS          VALUE(S)
===================================================
a                     INT    [10]          0 0 0 0 0 0 0 0 0 0 ...
b                     INT    [10,10]       0 0 0 0 0 0 0 0 0 0 ...
c                     INT    [10,10,10]    0 0 0 0 0 0 0 0 0 0 ...
main                  INT    ---           0

```




