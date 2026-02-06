#pragma once
#include "utils.hpp"
#include <algorithm>
#include <vector>

class IEconomyObject {
public:
  virtual ~IEconomyObject() = default;
  virtual void update(float dt) = 0;
  std::string uuid;
};

class EconomyObject : public IEconomyObject {
public:
  // Replaced template constants with constructor parameters
  EconomyObject(double defaultValue = 0.0f, int historyLength = 64,
                double baseLevel = 1.0f, const char *upgradeLevelData = nullptr,
                const char *valueIncreaseData = nullptr,
                const char *name = nullptr)
      : value(defaultValue), level(baseLevel),
        history(historyLength, defaultValue), minValue(defaultValue),
        maxValue(defaultValue) // Initialize vector size
  {
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
    uuid = util::uuid::generate_uuid_v4();
    if (name == nullptr) {
      this->name = uuid;
    } else {
      this->name = name;
    }
  }

  void update(float dt) override {
    value = rateIncreaseFormula.evaluate({value, level * dt});

    // Using vector-based utility or manual shift
    util::pushToBackOfVector(history, value);

    minValue = *std::min_element(history.begin(), history.end());
    maxValue = *std::max_element(history.begin(), history.end());
  };

  float getValueForLevelUpgrade(float LVup = 1.0f) {
    return upgradeLevelFormula.evaluate({level + LVup, 0.0f});
  }

  int getHistoryLength() const { return static_cast<int>(history.size()); }

  float value;
  float level;

  std::vector<float> history;
  float minValue;
  float maxValue;
  util::LogicEvaluator upgradeLevelFormula;
  util::LogicEvaluator rateIncreaseFormula;
  std::string name;
};

// Stock is now just a simple derived class or a specific configuration
class Stock : public EconomyObject {
public:
  Stock(float defaultValue = 0.0f, int historyLength = 64)
      : EconomyObject(defaultValue, historyLength) {}
};
