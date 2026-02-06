#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>
#include <cstdio>
#include <format>
#include <string>
namespace gui {
inline void floatInput(float &f, float min, float max, std::string label,
                       float step = 0.1f) {
  ImGui::InputFloat((label + "##input").c_str(), &f, step);
  f = std::clamp(f, min, max);
}
inline void doubleInput(double &f, double min, double max, std::string label,
                        double step = 0.1) {
  ImGui::InputDouble((label + "##input").c_str(), &f, step);
  f = std::clamp(f, min, max);
}

template <typename... Args>
inline bool buttonFormat(std::format_string<Args...> fmt, ImVec2 size,
                         Args &&...args) {
  char btnLabel[256];
  auto res = std::format_to_n(btnLabel, sizeof(btnLabel) - 1, fmt,
                              std::forward<Args>(args)...);
  *res.out = '\0';
  return ImGui::Button(btnLabel, size);
}

inline void setupFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}
}; // namespace gui
