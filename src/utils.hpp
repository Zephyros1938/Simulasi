#pragma once
#include "functionlang.hpp"
#include <algorithm>
#include <functional>
#include <random>
#include <sstream>

namespace util {
namespace uuid {
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string generate_uuid_v4() {
  std::stringstream ss;
  int i;
  ss << std::hex;
  for (i = 0; i < 8; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 4; i++) {
    ss << dis(gen);
  }
  ss << "-4";
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for (i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for (i = 0; i < 12; i++) {
    ss << dis(gen);
  };
  return ss.str();
}
} // namespace uuid

template <typename T, int S> void pushToBackOfArray(T (&array)[S], T val) {
  std::copy(array + 1, array + S, array);
  array[S - 1] = val;
}

template <typename T, int S> T minElement(T (&array)[S]) {
  int n = sizeof(array) / sizeof(array[0]);
  T minVal = array[0];

  for (int i = 0; i < n; i++) {
    if (array[i] < minVal) {
      minVal = array[i];
    }
  }

  return minVal;
}

template <typename T, int S> T maxElement(T (&array)[S]) {
  int n = sizeof(array) / sizeof(array[0]);
  T maxVal = array[0];

  for (int i = 0; i < n; i++) {
    if (array[i] > maxVal)
      maxVal = array[i];
  }

  return maxVal;
}

template <typename T, int S> void rotateArray(T (&array)[S], int k) {
  k %= S;
  if (k == 0)
    return;

  std::reverse(array, array + k);     // Reverse first k
  std::reverse(array + k, array + S); // Reverse the rest
  std::reverse(array, array + S);     // Reverse all
}

template <typename T, int S> void reverseArray(T (&array)[S]) {
  int i = 0, j = S - 1;
  while (i < j) {
    T temp = array[i];
    array[i] = array[j];
    array[j] = temp;
    i++;
    j--;
  }
}

class LogicEvaluator {
private:
  std::function<float(const std::vector<float> &)> formula;
  std::string rawSource;

public:
  LogicEvaluator(const std::string &source = "0") : rawSource(source) {
    const char *ptr = rawSource.c_str();
    formula = functionlang::parseExpression(ptr);
  }

  float evaluate(const std::vector<float> &args) const { return formula(args); }

  std::string getSource() const { return rawSource; }

  void updateFormula(const std::string &newSource) {
    rawSource = newSource;
    const char *ptr = rawSource.c_str();
    formula = functionlang::parseExpression(ptr);
  }
};

} // namespace util
