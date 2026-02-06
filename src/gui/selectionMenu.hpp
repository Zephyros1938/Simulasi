#pragma once

#include "economy/base.hpp"
#include "imgui.h"
#include <cstddef>
#include <string>
#include <vector>
namespace gui {
namespace selectionMenu {
template <typename T> class ISelectionMenu {
protected:
  std::vector<T> *selectionRef;
  std::vector<const char *> selectionNames;
  size_t selectionIndex;
  const char *noSelectionText;

  virtual std::string getPreviewName() {

    if (selectionRef->empty())
      return noSelectionText;
    else if (selectionIndex >= 0 && selectionIndex < selectionRef->capacity())
      return selectionNames[selectionIndex];

    return noSelectionText;
  }
  virtual std::string getItemName(size_t index) = 0;

public:
  ISelectionMenu(std::vector<T> *toSelect, const char *noSelectionText = "None")
      : selectionRef(toSelect), noSelectionText(noSelectionText),
        selectionIndex(0) {}
  virtual ~ISelectionMenu() = default;

  void display(const char *label = "Select") {
    std::string preview = getPreviewName();

    if (ImGui::BeginCombo(label, preview.c_str())) {
      for (size_t n = 0; n < selectionRef->size(); n++) {
        const bool isSelected = (selectionIndex == n);

        // We call getItemName(n) here so every row gets its own name
        std::string name = getItemName(n);

        if (ImGui::Selectable(name.c_str(), isSelected)) {
          selectionIndex = n;
        }

        if (isSelected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }

  T &getSelectedItem() { return selectionRef[selectionIndex]; };
  size_t getIndex() { return selectionIndex; }
};

class EconomyObjectSelectionMenu : public ISelectionMenu<EconomyObject> {
public: // Made public so you can actually instantiate it!
  EconomyObjectSelectionMenu(std::vector<EconomyObject> *toSelect,
                             const char *noSelectionText = "None")
      : ISelectionMenu<EconomyObject>(toSelect, noSelectionText) {}

  std::string getPreviewName() override {
    if (!selectionRef || selectionRef->empty())
      return noSelectionText;

    // Use .size() rather than .capacity() for bounds checking
    if (selectionIndex < selectionRef->size()) {
      return (*selectionRef)[selectionIndex].name; // Assuming getName() exists
    }
    return noSelectionText;
  }

  std::string getItemName(size_t index) override {
    return (*selectionRef)[index].name;
  }
};
}; // namespace selectionMenu
}; // namespace gui
