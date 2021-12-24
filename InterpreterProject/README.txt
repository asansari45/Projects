Interpreter Project

Built with Microsoft Visual Studio Enterprise 2019 Version 16.8.5
To build the project perform the following steps:
1.  C:\Users\asansari\FlexBison\win_bison.exe --defines=InterpreterParser.hpp --output=InterpreterParser.cpp Interpreter.y
2.  C:\Users\asansari\FlexBison\win_flex.exe --c++ --header-file=InterpreterLex.hpp Interpreter.l
3.  In the generated lex.yy.c replace fileno() call with _fileno() for widows.
3.  Build project and run

The grammar/production rules for the interpreter are found in the Interpreter.y 
file.  The tokens are found in the Interpreter.l file.  For more information on 
flex and bison, please see https://www.gnu.org/software/bison/.  The flex and bison
binaries were downloaded from:  https://sourceforge.net/projects/winflexbison/.

The Interpreter does have a help command.
