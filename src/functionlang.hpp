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

const std::function<float(float, float)> parseExpression(const char *&ptr) {
  if (ptr == nullptr || *ptr == '\0') {
    return [](float l, float v) { return 0.0f; };
  }
  while (ptr && (*ptr == ' ' || *ptr == '\t'))
    ptr++;
  char op = *ptr++;

  if (op == IN_1)
    return [](float l, float v) { return l; };
  if (op == IN_2)
    return [](float l, float v) { return v; };
  if (std::isdigit(op) || op == '.' || op == '-') {
    ptr--;
    float val = strtof(ptr, const_cast<char **>(&ptr));
    return [val](float l, float v) { return val; };
  }
  auto arg1 = parseExpression(ptr);

  if (std::ranges::contains(UNARY_OPS, op)) {
    switch (op) {
    case UNARY_OPS_ENUM::LOG:
      return [arg1](float l, float v) { return std::log(arg1(l, v)); };
    case UNARY_OPS_ENUM::LOG2:
      return [arg1](float l, float v) { return std::log2(arg1(l, v)); };
    case UNARY_OPS_ENUM::LOG10:
      return [arg1](float l, float v) { return std::log10(arg1(l, v)); };
    case UNARY_OPS_ENUM::SQRT:
      return [arg1](float l, float v) { return std::sqrt(arg1(l, v)); };
    case UNARY_OPS_ENUM::CBRT:
      return [arg1](float l, float v) { return std::cbrt(arg1(l, v)); };
    case UNARY_OPS_ENUM::SIN:
      return [arg1](float l, float v) { return std::sin(arg1(l, v)); };
    case UNARY_OPS_ENUM::COS:
      return [arg1](float l, float v) { return std::cos(arg1(l, v)); };
    case UNARY_OPS_ENUM::ABS:
      return [arg1](float l, float v) { return std::abs(arg1(l, v)); };
    case UNARY_OPS_ENUM::NOT:
      return [arg1](float l, float v) {
        return arg1(l, v) <= 0.0f ? 1.0f : -1.0f;
      };
    default:
      break;
    }
  } else if (std::ranges::contains(BINARY_OPS, op)) {
    if (*ptr == ',')
      ptr++;
    auto arg2 = parseExpression(ptr);
    switch (op) {
    case BINARY_OPS_ENUM::MUL:
      return [arg1, arg2](float l, float v) { return arg1(l, v) * arg2(l, v); };
    case BINARY_OPS_ENUM::DIV:
      return [arg1, arg2](float l, float v) {
        float b = arg2(l, v);
        return b == 0.0f ? 0.0f : arg1(l, v) / b;
      };
    case BINARY_OPS_ENUM::ADD:
      return [arg1, arg2](float l, float v) { return arg1(l, v) + arg2(l, v); };
    case BINARY_OPS_ENUM::SUB:
      return [arg1, arg2](float l, float v) { return arg1(l, v) - arg2(l, v); };
    case BINARY_OPS_ENUM::POW:
      return [arg1, arg2](float l, float v) {
        return std::pow(arg1(l, v), arg2(l, v));
      };
    case BINARY_OPS_ENUM::MIN:
      return [arg1, arg2](float l, float v) {
        return std::min(arg1(l, v), arg2(l, v));
      };
    case BINARY_OPS_ENUM::MAX:
      return [arg1, arg2](float l, float v) {
        return std::max(arg1(l, v), arg2(l, v));
      };
    case BINARY_OPS_ENUM::LOG_N:
      return [arg1, arg2](float l, float v) {
        float base = arg1(l, v);
        float value = arg2(l, v);

        if (value <= 0.0 || base <= 0.0 || base == 1.0)
          return 0.0f;

        return std::log(value) / std::log(base);
      };
    case BINARY_OPS_ENUM::LT:
      return [arg1, arg2](float l, float v) {
        return arg1(l, v) < arg2(l, v) ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::GT:
      return [arg1, arg2](float l, float v) {
        return arg1(l, v) > arg2(l, v) ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::EQ:
      return [arg1, arg2](float l, float v) {
        return std::abs(arg1(l, v) - arg2(l, v)) < 0.00001f ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::NE:
      return [arg1, arg2](float l, float v) {
        return std::abs(arg1(l, v) - arg2(l, v)) > 0.00001f ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::L_AND:
      return [arg1, arg2](float l, float v) {
        return (arg1(l, v) > 0.0f) && (arg2(l, v) > 0.0f) ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::L_OR:
      return [arg1, arg2](float l, float v) {
        return (arg1(l, v) > 0.0f) || (arg2(l, v) > 0.0f) ? 1.0f : -1.0f;
      };
    case BINARY_OPS_ENUM::MOD:
      return [arg1, arg2](float l, float v) {
        float b = arg2(l, v);
        return (b == 0.0f) ? 0.0f : std::fmod(arg1(l, v), b);
      };
    default:
      break;
    }
  } else if (std::ranges::contains(TERNARY_OPS, op)) {
    if (*ptr == ',')
      ptr++;
    auto arg2 = parseExpression(ptr);
    if (*ptr == ',')
      ptr++;
    auto arg3 = parseExpression(ptr);
    switch (op) {
    case TERNARY_OPS_ENUM::WHETHER:
      return [arg1, arg2, arg3](float l, float v) {
        return (arg1(l, v) > 0.0f) ? arg2(l, v) : arg3(l, v);
      };
    default:
      break;
    }
  }

  return [](float l, float v) { return 0.0f; };
}
} // namespace functionlang
