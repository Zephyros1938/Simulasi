#pragma once
#include "functionlang.hpp"
#include <algorithm>
#include <cfloat>
#include <climits>
#include <functional>
#include <random>
#include <sstream>

namespace util {

namespace rand {
class Random {
  static std::mt19937 &get_engine() {
    thread_local std::mt19937 engine(std::random_device{}());
    return engine;
  }

public:
  static int get_int(int min = INT_MIN, int max = INT_MAX) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(get_engine());
  }

  static unsigned int get_unsigned_int(unsigned int min = 0,
                                       unsigned int max = UINT_MAX) {
    std::uniform_int_distribution<unsigned int> dist(min, max);
    return dist(get_engine());
  }

  static float get_float(float min = 0.0f, float max = 1.0f) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(get_engine());
  }

  static double get_double(double min = 0.0, double max = 1.0) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(get_engine());
  }
};
}; // namespace rand

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

template <typename T> void pushToBackOfVector(std::vector<T> &vec, T val) {
  if (vec.empty())
    return;
  std::copy(vec.begin() + 1, vec.end(), vec.begin());
  vec.back() = val;
}

// 2. Find Minimum Element
template <typename T> T minElement(const std::vector<T> &vec) {
  // Standard library approach: return *std::min_element(vec.begin(),
  // vec.end());
  T minVal = vec[0];
  for (const auto &item : vec) {
    if (item < minVal)
      minVal = item;
  }
  return minVal;
}

// 3. Find Maximum Element
template <typename T> T maxElement(const std::vector<T> &vec) {
  T maxVal = vec[0];
  for (const auto &item : vec) {
    if (item > maxVal)
      maxVal = item;
  }
  return maxVal;
}

// 4. Rotate Vector (Left rotation by k)
template <typename T> void rotateVector(std::vector<T> &vec, int k) {
  int n = vec.size();
  if (n == 0)
    return;
  k %= n;
  if (k == 0)
    return;

  std::reverse(vec.begin(), vec.begin() + k);
  std::reverse(vec.begin() + k, vec.end());
  std::reverse(vec.begin(), vec.end());
}

// 5. Reverse Vector
template <typename T> void reverseVector(std::vector<T> &vec) {
  // You can also use std::reverse(vec.begin(), vec.end());
  int i = 0, j = vec.size() - 1;
  while (i < j) {
    std::swap(vec[i], vec[j]);
    i++;
    j--;
  }
}

class LogicEvaluator {
private:
  std::function<double(const std::vector<double> &)> formula;
  std::string rawSource;

public:
  LogicEvaluator(const std::string &source = "0") : rawSource(source) {
    const char *ptr = rawSource.c_str();
    formula = functionlang::parseExpression(ptr);
  }

  float evaluate(const std::vector<double> &args) const {
    return formula(args);
  }

  std::string getSource() const { return rawSource; }

  void updateFormula(const std::string &newSource) {
    rawSource = newSource;
    const char *ptr = rawSource.c_str();
    formula = functionlang::parseExpression(ptr);
  }
};

} // namespace util
