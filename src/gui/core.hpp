#include "imgui.h"
#include <algorithm>
#include <string>
namespace gui {
inline void floatInput(float &f, float min, float max, std::string label,
                       float step = 0.1f) {
  ImGui::InputFloat((label + "##input").c_str(), &f, step);
  f = std::clamp(f, min, max);
  ImGui::SameLine();
  ImGui::SliderScalar((label + "##slider").c_str(), ImGuiDataType_Float, &f,
                      &min, &max);
}
inline void doubleInput(double &f, double min, double max, std::string label,
                        double step = 0.1) {
  ImGui::InputDouble((label + "##input").c_str(), &f, step);
  f = std::clamp(f, min, max);
  ImGui::SameLine();
  ImGui::SliderScalar((label + "##slider").c_str(), ImGuiDataType_Double, &f,
                      &min, &max);
}
}; // namespace gui
