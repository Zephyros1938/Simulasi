#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>

namespace functionlang {

const char VERSION[] = "0.2.1";

const char IN_1 = '(';
const char IN_2 = ')';

enum UNARY_OPS_ENUM {
  LOG = 'l',
  LOG2 = 'L',
  LOG10 = 'g',
  SQRT = 's',
  CBRT = 'S',
  SIN = 'i',
  COS = 'I',
  ABS = 'a',
  NOT = '!'
};
enum BINARY_OPS_ENUM {
  MUL = '*',
  DIV = '/',
  ADD = '+',
  SUB = '_',
  POW = '^',
  MIN = 'm',
  MAX = 'M',
  LOG_N = 'G',
  LT = '<',
  GT = '>',
  EQ = '=',
  NE = '\\',
  L_AND = '&',
  L_OR = '|',
  MOD = '%'
};
enum TERNARY_OPS_ENUM { WHETHER = '?' };

const char UNARY_OPS[] = {
    UNARY_OPS_ENUM::LOG,  UNARY_OPS_ENUM::LOG2, UNARY_OPS_ENUM::LOG10,
    UNARY_OPS_ENUM::SQRT, UNARY_OPS_ENUM::CBRT, UNARY_OPS_ENUM::SIN,
    UNARY_OPS_ENUM::COS,  UNARY_OPS_ENUM::ABS,  UNARY_OPS_ENUM::NOT};
const char BINARY_OPS[] = {
    BINARY_OPS_ENUM::MUL,   BINARY_OPS_ENUM::DIV,   BINARY_OPS_ENUM::ADD,
    BINARY_OPS_ENUM::SUB,   BINARY_OPS_ENUM::POW,   BINARY_OPS_ENUM::MIN,
    BINARY_OPS_ENUM::MAX,   BINARY_OPS_ENUM::LOG_N, BINARY_OPS_ENUM::LT,
    BINARY_OPS_ENUM::GT,    BINARY_OPS_ENUM::EQ,    BINARY_OPS_ENUM::NE,
    BINARY_OPS_ENUM::L_AND, BINARY_OPS_ENUM::L_OR,  BINARY_OPS_ENUM::MOD};
const char TERNARY_OPS[] = {TERNARY_OPS_ENUM::WHETHER};

using ExprFuncRet = const std::vector<float> &;
using ExprFunc = std::function<float(ExprFuncRet)>;

const ExprFunc parseExpression(const char *&ptr) {
  if (ptr == nullptr || *ptr == '\0') {
    return [](ExprFuncRet) { return 0.0f; };
  }
  while (ptr && (*ptr == ' ' || *ptr == '\t'))
    ptr++;
  char op = *ptr++;

  if (op == 'V') {
    // Parse the index number immediately following 'V'
    char *endPtr;
    int index = static_cast<int>(std::strtol(ptr, &endPtr, 10));

    // Move the global pointer forward to after the number
    ptr = endPtr;

    return [index](ExprFuncRet args) {
      // Safety check: ensure index exists in the provided vector
      if (index >= 0 && static_cast<size_t>(index) < args.size()) {
        return args[index];
      }
      return 0.0f; // Default if index is out of bounds
    };
  }
  if (std::isdigit(op) || op == '.' || op == '-') {
    ptr--;
    float val = strtof(ptr, const_cast<char **>(&ptr));
    return [val](ExprFuncRet) { return val; };
  }
  auto arg1 = parseExpression(ptr);

  if (std::ranges::contains(UNARY_OPS, op)) {
    return [arg1, op](ExprFuncRet args) {
      float v1 = arg1(args);
      switch (op) {
      case UNARY_OPS_ENUM::LOG:
        return std::log(v1);
      case UNARY_OPS_ENUM::LOG2:
        return std::log2(v1);
      case UNARY_OPS_ENUM::LOG10:
        return std::log10(v1);
      case UNARY_OPS_ENUM::SQRT:
        return std::sqrt(v1);
      case UNARY_OPS_ENUM::CBRT:
        return std::cbrt(v1);
      case UNARY_OPS_ENUM::SIN:
        return std::sin(v1);
      case UNARY_OPS_ENUM::COS:
        return std::cos(v1);
      case UNARY_OPS_ENUM::ABS:
        return std::abs(v1);
      case UNARY_OPS_ENUM::NOT:
        return v1 <= 0.0f ? 1.0f : -1.0f;
      default:
        return 0.0f;
      };
    };
  } else if (std::ranges::contains(BINARY_OPS, op)) {
    if (*ptr == ',')
      ptr++;
    auto arg2 = parseExpression(ptr);
    return [arg1, arg2, op](ExprFuncRet args) {
      float v1 = arg1(args);
      float v2 = arg2(args);
      switch (op) {
      case BINARY_OPS_ENUM::MUL:
        return v1 * v2;
      case BINARY_OPS_ENUM::DIV:
        return v2 == 0.0f ? 0.0f : v1 / v2;
      case BINARY_OPS_ENUM::ADD:
        return v1 + v2;
      case BINARY_OPS_ENUM::SUB:
        return v1 - v2;
      case BINARY_OPS_ENUM::POW:
        return std::pow(v1, v2);
      case BINARY_OPS_ENUM::MIN:
        return std::min(v1, v2);
      case BINARY_OPS_ENUM::MAX:
        return std::max(v1, v2);
      case BINARY_OPS_ENUM::LOG_N:
        if (v2 <= 0.0 || v1 <= 0.0 || v1 == 1.0)
          return 0.0f;
        return std::log(v2) / std::log(v1);
      case BINARY_OPS_ENUM::LT:
        return v1 < v2 ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::GT:
        return v1 > v2 ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::EQ:
        return std::abs(v1 - v2) < 0.00001f ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::NE:
        return std::abs(v1 - v2) > 0.00001f ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::L_AND:
        return (v1 > 0.0f) && (v2 > 0.0f) ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::L_OR:
        return (v1 > 0.0f) || (v2 > 0.0f) ? 1.0f : -1.0f;
      case BINARY_OPS_ENUM::MOD:
        return v2 = 0.0f ? 0.0f : std::fmod(v1, v2);
      default:
        return 0.0f;
      };
    };

  } else if (std::ranges::contains(TERNARY_OPS, op)) {
    if (*ptr == ',')
      ptr++;
    auto arg2 = parseExpression(ptr);
    if (*ptr == ',')
      ptr++;
    auto arg3 = parseExpression(ptr);
    return [arg1, arg2, arg3, op](ExprFuncRet args) {
      float v1 = arg1(args);
      float v2 = arg2(args);
      float v3 = arg3(args);
      switch (op) {
      case TERNARY_OPS_ENUM::WHETHER:
        return v1 > 0.0f ? v2 : v3;
      default:
        return 0.0f;
      };
    };
  }

  return [](ExprFuncRet) { return 0.0f; };
}
} // namespace functionlang
