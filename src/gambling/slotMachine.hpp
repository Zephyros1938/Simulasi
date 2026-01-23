#pragma once
#include "utils.hpp"
#include <cstdlib>
#include <vector>

namespace gambling {

template <char SlotSize, typename T> class SlotMachine {
public:
  SlotMachine() {
    for (int x = 0; x < SlotSize; x++) {
      for (int y = 0; y < 5; y++) {
        slots[x][y] = y;
      }
    }

    for (auto &slot : slots) {
      util::rotateArray(slot, (int)(rand() % 100 + 1));
    }
  }

  void roll(float &bid) {
    for (auto &slot : slots) {
      util::rotateArray(slot, (int)(rand() % 100 + 1));
    }

    std::vector<int> middleSlot;

    for (int x = 0; x < SlotSize; x++) {
      middleSlot.push_back(slots[x][2]);
    }

    int totalRating = 0;

    for (int v : middleSlot) {
      totalRating += v;
    }

    bool shouldBonus = true;
    for (int x = 0; x < SlotSize; x++) {
      if (slots[x][2] != slots[0][2]) {
        shouldBonus = false;
        break;
      }
    }

    bid *= (std::log(totalRating) + 1) * (shouldBonus ? 1.5 : 1.0);
  }

private:
  int slots[SlotSize][5];
};
}; // namespace gambling
