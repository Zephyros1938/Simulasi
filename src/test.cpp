#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>

namespace functionlang {
std::function<float(float, float)> parseExpression(const char *&ptr) {
  if (ptr == nullptr || *ptr == '\0') {
    return [](float l, float v) { return 0.0f; };
  }
  char op = *ptr++;

  if (op == 'L')
    return [](float l, float v) { return l; };
  if (op == 'V')
    return [](float l, float v) { return v; };
  if (std::isdigit(op) || op == '-') {
    ptr--;
    float val = strtof(ptr, const_cast<char **>(&ptr));
    return [val](float l, float v) { return val; };
  }

  auto arg1 = parseExpression(ptr);
  if (*ptr == ',')
    ptr++;
  if (op == '*') {
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2](float l, float v) { return arg1(l, v) * arg2(l, v); };
  }
  if (op == '/') {
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2](float l, float v) {
      return (arg2 != 0) ? arg1(l, v) / arg2(l, v) : 0.0f;
    };
  }
  if (op == '+') {
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2](float l, float v) { return arg1(l, v) + arg2(l, v); };
  }
  if (op == '_') {
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2](float l, float v) { return arg1(l, v) - arg2(l, v); };
  }
  if (op == 'p') {
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2](float l, float v) {
      return std::pow(arg1(l, v), arg2(l, v));
    };
  }

  if (op == 'l') {
    return [arg1](float l, float v) { return std::log2(arg1(l, v)); };
  }

  return [](float l, float v) { return 0.0f; };
}
} // namespace functionlang

int main() {
  const char *c = "+1,_-1,-2";
  std::cout << functionlang::parseExpression(c)(10, 5) << std::endl;
  return 0;
}
