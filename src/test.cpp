#include "functionlang.hpp"

#include <format>
#include <iostream>
#include <string>

const std::string help_string = std::format("FunctionLang Version {}\n\
Unary Operators:\n\
- l : log\n\
- L : log2\n\
- g : log10\n\
- s : sqrt\n\
- S : cbrt\n\
- i : sin\n\
- I : cos\n\
- a : abs\n\
- ! : Logical NOT\n\
Binary Operators:\n\
- * : MUL\n\
- / : DIV\n\
- + : ADD\n\
- _ : SUB\n\
- ^ : POW\n\
- m : MIN\n\
- M : MAX\n\
- G : LOG_N (b,v)\n\
- < : LESS THAN\n\
- > : GREATER THAN\n\
- = : EQUAL\n\
- \\ : NEQUAL\n\
- & : Logical AND\n\
- | : Logical OR\n\
- % : Modulus\n\
Ternary Operators:\n\
- ? : Ternary Operator\n\
Usage:\n\
- [op][x],[y],[z] for ternary operators\n\
- [op][x],[y]     for binary operators\n\
- [op][x]         for unary operators\n\
- you can stack operators like [op][x],[op][x],[y]\n\
Examples:\n\
Input         | Output      | Math Equivilant\n\
+1,2          | 3           | 1+2\n\
s16           | 4           | sqrt(16)\n\
^2,3          | 8           | 2^3\n\
*2,s^2,2      | 4           | 2*sqrt(2^2)\n\
L^2,3         | 3           | log_2(2^3)\n\
G10,100       | 2           | log_10(100)\n\
g100          | 2           | log10(100)\n\
",
                                            functionlang::VERSION);

int main() {
  std::string input_buffer;
  const char *pt = nullptr;

  std::cout << ":q to exit | :h for help" << std::endl;

  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, input_buffer) || input_buffer == ":q") {
      break;
    }
    if (input_buffer == ":h") {
      std::cout << help_string << std::endl;
      continue;
    }

    pt = input_buffer.c_str();

    std::cout << functionlang::parseExpression(pt)({10, 5}) << std::endl;
  }
  return 0;
}
