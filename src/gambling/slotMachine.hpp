#pragma once
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>

namespace gambling {

template <std::size_t SlotSize> class SlotMachine {
public:
  explicit SlotMachine(float baseBid)
      : m_minBid(baseBid), m_cachedBid(baseBid), m_gen(std::random_device{}()) {

    // Initialize symbols: 0 is common, 4 is rare (Jackpot)
    for (std::size_t x = 0; x < SlotSize; ++x) {
      for (int y = 0; y < 5; ++y) {
        m_slots[x][y] = y;
      }
      spinColumn(x);
    }
  }

  // --- GUI Helpers ---
  void setBid(float amount) { m_cachedBid = std::max(amount, m_minBid); }

  float *getCachedBid() { return &m_cachedBid; }
  float getCachedBidV() { return m_cachedBid; }
  float *getMinBid() { return &m_minBid; }

  // --- Core Logic ---
  int roll(float &playerBalance) {
    if (playerBalance < m_cachedBid) {
      return -1; // Insufficient Funds
    }

    playerBalance -= m_cachedBid;

    for (std::size_t x = 0; x < SlotSize; ++x) {
      spinColumn(x);
    }

    // --- Payout Calculation ---
    int firstSymbol = m_slots[0][2];
    bool allMatch = true;
    int matchCount = 1;

    for (std::size_t x = 1; x < SlotSize; ++x) {
      if (m_slots[x][2] == firstSymbol) {
        matchCount++;
      } else {
        allMatch = false;
      }
    }

    double multiplier = 0.0;

    if (allMatch) {
      // Jackpot: Payout scales exponentially with the symbol's ID
      // Match three '4's: pow(5, 2.5) * 5 = ~279x multiplier
      multiplier = std::pow(firstSymbol + 1, 2.5) * 5.0;
    } else if (matchCount >= 2) {
      // "Near Miss" or Partial Match: Return some money to keep player engaged
      multiplier = (firstSymbol + 1) * 0.4;
    } else {
      // House wins.
      multiplier = 0.0;
    }

    playerBalance += static_cast<float>(m_cachedBid * multiplier);

    // Return the center symbol of the first column as a "rating" for the UI
    return m_slots[0][2];
  }

private:
  void spinColumn(std::size_t colIndex) {
    // Weighted Distribution: 0 and 1 are very common, 4 is very rare.
    // This ensures the "big" symbols don't hit the center row too often.
    std::discrete_distribution<int> weightDist({40, 30, 15, 10, 5});

    // Rotate the column by a random amount
    std::uniform_int_distribution<int> rotDist(1, 50);
    util::rotateArray(m_slots[colIndex], rotDist(m_gen));

    // Set the middle row result based on weights for "true" gambling feel
    m_slots[colIndex][2] = weightDist(m_gen);
  }

  float m_minBid;
  float m_cachedBid;
  std::mt19937 m_gen;
  int m_slots[SlotSize][5];
};

} // namespace gambling
