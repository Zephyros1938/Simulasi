#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>

namespace functionlang {

const char VERSION[] = "0.2.0";

enum UNARY_OPS_ENUM {
  log = 'l',
  log2 = 'L',
  log10 = 'g',
  sqrt = 's',
  cbrt = 'S',
  sin = 'i',
  cos = 'I',
  abs = 'a',
  no = '!'
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

enum TERNARY_OPS_ENUM { whether = '?' };

const char UNARY_OPS[] = {
    UNARY_OPS_ENUM::log,  UNARY_OPS_ENUM::log2, UNARY_OPS_ENUM::log10,
    UNARY_OPS_ENUM::sqrt, UNARY_OPS_ENUM::cbrt, UNARY_OPS_ENUM::sin,
    UNARY_OPS_ENUM::cos,  UNARY_OPS_ENUM::abs,  UNARY_OPS_ENUM::no};
const char BINARY_OPS[] = {
    BINARY_OPS_ENUM::MUL,   BINARY_OPS_ENUM::DIV,   BINARY_OPS_ENUM::ADD,
    BINARY_OPS_ENUM::SUB,   BINARY_OPS_ENUM::POW,   BINARY_OPS_ENUM::MIN,
    BINARY_OPS_ENUM::MAX,   BINARY_OPS_ENUM::LOG_N, BINARY_OPS_ENUM::LT,
    BINARY_OPS_ENUM::GT,    BINARY_OPS_ENUM::EQ,    BINARY_OPS_ENUM::NE,
    BINARY_OPS_ENUM::L_AND, BINARY_OPS_ENUM::L_OR,  BINARY_OPS_ENUM::MOD};
const char TERNARY_OPS[] = {TERNARY_OPS_ENUM::whether};

const std::function<float(float, float)> parseExpression(const char *&ptr) {
  if (ptr == nullptr || *ptr == '\0') {
    return [](float l, float v) { return 0.0f; };
  }
  while (ptr && *ptr == ' ' && *ptr == '\t')
    ptr++;
  char op = *ptr++;

  if (op == '(')
    return [](float l, float v) { return l; };
  if (op == ')')
    return [](float l, float v) { return v; };
  if (std::isdigit(op) || op == '.' || op == '-') {
    ptr--;
    float val = strtof(ptr, const_cast<char **>(&ptr));
    return [val](float l, float v) { return val; };
  }
  auto arg1 = parseExpression(ptr);

  if (std::ranges::contains(UNARY_OPS, op)) {
    switch (op) {
    case UNARY_OPS_ENUM::log:
      return [arg1](float l, float v) { return std::log(arg1(l, v)); };
    case UNARY_OPS_ENUM::log2:
      return [arg1](float l, float v) { return std::log2(arg1(l, v)); };
    case UNARY_OPS_ENUM::log10:
      return [arg1](float l, float v) { return std::log10(arg1(l, v)); };
    case UNARY_OPS_ENUM::sqrt:
      return [arg1](float l, float v) { return std::sqrt(arg1(l, v)); };
    case UNARY_OPS_ENUM::cbrt:
      return [arg1](float l, float v) { return std::cbrt(arg1(l, v)); };
    case UNARY_OPS_ENUM::sin:
      return [arg1](float l, float v) { return std::sin(arg1(l, v)); };
    case UNARY_OPS_ENUM::cos:
      return [arg1](float l, float v) { return std::cos(arg1(l, v)); };
    case UNARY_OPS_ENUM::abs:
      return [arg1](float l, float v) { return std::abs(arg1(l, v)); };
    case UNARY_OPS_ENUM::no:
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
        return std::fmod(arg1(l, v), arg2(l, v));
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
    case TERNARY_OPS_ENUM::whether:
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
