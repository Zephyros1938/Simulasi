#pragma once
#include "utils.hpp"
template <typename T, T DefaultValue, int HistoryLength> class EconomyObject {
public:
  EconomyObject(float baseLevel = 1.0f, const char *upgradeLevelData = nullptr,
                const char *valueIncreaseData = nullptr,
                std::string name = util::uuid::generate_uuid_v4())
      : value(DefaultValue), level(baseLevel), minValue(DefaultValue),
        maxValue(DefaultValue), name(name)
  // Initialize directly to the lambda
  {
    std::fill_n(history, HistoryLength, DefaultValue);
    if (upgradeLevelData != nullptr) {
      upgradeLevelFormula = util::LogicEvaluator(upgradeLevelData);
    } else {
      upgradeLevelFormula = util::LogicEvaluator("*10,^1.15,V0");
    }
    if (valueIncreaseData != nullptr) {
      rateIncreaseFormula = util::LogicEvaluator(valueIncreaseData);
    } else {
      rateIncreaseFormula = util::LogicEvaluator("+V0,V1");
    }
  }

  void update(float dt) {
    value = rateIncreaseFormula.evaluate({value, level * dt});
    util::pushToBackOfArray(history, value);
    minValue = util::minElement(history);
    maxValue = util::maxElement(history);
  };

  T getValue() const { return value; }
  float getLevel() const { return level; }
  const T *getHistory() const { return history; }
  constexpr int getHistoryLength() const { return HistoryLength; }
  T getMinHistoryV() const { return minValue; }
  T getMaxHistoryV() const { return maxValue; }

  void setLevel(float v) { level += v; }
  void incrLevel(float what = 1) { level += what; }
  void decrLevel(float what = 1) { level -= what; }

  void setValue(T v) { value = v; }
  void incrValue(T what = 1) { value += what; }
  void decrValue(T what = 1) { value -= what; }

  T getValueForLevelUpgrade(float LVup = 1.0) {
    return upgradeLevelFormula.evaluate({level + LVup, 0.0});
  }
  std::string getLevelCostFormula() { return upgradeLevelFormula.getSource(); }
  std::string getRateIncreaseFormula() {
    return rateIncreaseFormula.getSource();
  }
  std::string getName() { return name; }

private:
  T value;
  float level;
  T history[HistoryLength];
  T minValue;
  T maxValue;
  util::LogicEvaluator upgradeLevelFormula;
  util::LogicEvaluator rateIncreaseFormula;
  std::string name;
};

template <float DefaultValue = 0, int HistoryLength = 64>
class Stock : public EconomyObject<float, DefaultValue, HistoryLength> {};

class Economy {
public:
  EconomyObject<float, 10.0f, 1024> baseShare =
      EconomyObject<float, 10.0f, 1024>(1.0, nullptr, nullptr, "Basic Share");
  void update(double dt) { baseShare.update(dt); }
};
