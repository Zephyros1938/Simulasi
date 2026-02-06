#include "functionlang.hpp"

#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <vector>

namespace Color {
const std::string Reset = "\033[0m";
const std::string Red = "\033[31m";
const std::string Green = "\033[32m";
const std::string Yellow = "\033[33m";
const std::string Blue = "\033[34m";
const std::string Cyan = "\033[36m";
const std::string Bold = "\033[1m";
} // namespace Color

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
- ~ : Round (round V0 by V1)\n\
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

  std::vector<double> values;
  values.resize(256, 0.0);

  std::cout << ":q to exit | :h for help | :s V[n] [expr] | V[0-255] to index "
               "value store"
            << std::endl;

  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, input_buffer) || input_buffer == ":q") {
      break;
    }
    if (input_buffer == ":h") {
      std::cout << help_string << std::endl;
      continue;
    }
    if (input_buffer.starts_with(":s")) {
      try {
        size_t v_pos = input_buffer.find('V');
        size_t space_pos = input_buffer.find(' ', v_pos);
        if (v_pos != std::string::npos && space_pos != std::string::npos) {
          int index =
              std::stoi(input_buffer.substr(v_pos + 1, space_pos - v_pos - 1));
          std::string expr_part = input_buffer.substr(space_pos + 1);
          auto cs = expr_part.c_str();
          double result = functionlang::parseExpression(cs)({10, 5});
          if (index >= 0 && index < (int)values.size()) {
            values[index] = result;
            std::cout << "V" << index << " = " << result << std::endl;
          } else {
            std::cerr << Color::Red << "Error: Index V" << index
                      << " out of range." << Color::Reset << std::endl;
          }
        }
      } catch (const std::exception &e) {
        std::cerr << Color::Red << "Error parsing setter: " << e.what()
                  << Color::Reset << std::endl;
      }
      continue;
    }
    pt = input_buffer.c_str();

    std::cout << functionlang::parseExpression(pt)(values) << std::endl;
  }
  return 0;
}
