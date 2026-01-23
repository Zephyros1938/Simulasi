#include "economy/base.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functionlang.hpp>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <vector>

namespace initialization {
using namespace std;

vector<uint32_t> ImGuiFlags = {ImGuiConfigFlags_DockingEnable,
                               ImGuiConfigFlags_IsSRGB};
map<uint32_t, uint32_t> GlfwFlags = {
    {GLFW_CONTEXT_VERSION_MAJOR, 4},
    {GLFW_CONTEXT_VERSION_MINOR, 6},
    {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
    {GLFW_RESIZABLE, GLFW_FALSE}};
map<uint32_t, uint32_t> GlfwInitFlags{};
} // namespace initialization
namespace settings {
static glm::vec4 clearColor{0.5, 0.5, 0.5, 1.0};
static char windowTitle[256] = "Simulasi";
const int width = 1280;
const float aspectx = 16;
const float aspecty = 9;
const int height = width * (aspecty / aspectx);
} // namespace settings
namespace game_data {
Economy economy;
} // namespace game_data

void initGlfw();
void initImGui();
int cleanup();
GLFWwindow *window = nullptr;

int main() {
  initGlfw();
  initImGui();

  glfwSetWindowSize(window, settings::width, settings::height);
  float lastFrame = 0.0f;
  float deltaTime = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    game_data::economy.update(deltaTime);

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    {
      ImGui::Begin("Main Menu");
      ImGui::SeparatorText("General");
      if (ImGui::Button("Quit")) {
        glfwSetWindowShouldClose(window, true);
      }

      ImGui::End();
    }
    {
      ImGui::Begin("Economy Management", nullptr, ImGuiWindowFlags_NoCollapse);

      auto *e = &game_data::economy.baseShare;
      float currentVal = e->getValue();
      float requiredSpend = e->getValueForLevelUpgrade();
      bool canAfford = currentVal >= requiredSpend;

      // --- Header Section ---
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Market Status");
      ImGui::Separator();

      // --- Graph Section ---
      ImGui::PlotLines(("##History" + e->getName()).c_str(), e->getHistory(),
                       e->getHistoryLength(), 0, e->getName().c_str(),
                       e->getMinHistoryV(), e->getMaxHistoryV(),
                       ImVec2(ImGui::GetContentRegionAvail().x, 120));

      // --- Stats Table ---
      if (ImGui::BeginTable("Stats", 2, ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Current Value:");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f", currentVal);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Growth Rate (LV):");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f / sec", e->getLevel());

        ImGui::EndTable();
      }
      ImGui::Spacing();
      ImGui::SeparatorText("Formulas");
      if (ImGui::BeginTable("##Formulas", 2, ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Name");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Formula");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Level Cost");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", e->getLevelCostFormula().c_str());

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Rate Increase");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", e->getRateIncreaseFormula().c_str());
        ImGui::EndTable();
      }

      ImGui::Spacing();
      ImGui::SeparatorText("Actions");

      ImGui::Spacing();

      // --- Upgrade Section ---
      // Change style for the upgrade button based on affordability
      if (!canAfford) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.1f, 1.0f));
      } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
      }

      char btnLabel[64];
      sprintf(btnLabel, "Upgrade Level (Cost: %.2f)", requiredSpend);
      if (ImGui::Button(btnLabel,
                        ImVec2(ImGui::GetContentRegionAvail().x, 30)) &&
          canAfford) {
        e->decrValue(requiredSpend);
        e->incrLevel();
      }
      ImGui::PopStyleColor();

      // Progress bar for the next upgrade
      float progress = std::clamp(currentVal / requiredSpend, 0.0f, 1.0f);
      ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0),
                         canAfford ? "READY TO UPGRADE"
                                   : "Accumulating Funds...");

      ImGui::End();
    }
    {
      ImGui::Begin("Debug");
      ImGui::SeparatorText("Debug");
      ImGui::Text("dt: %.2f", 1.0f / deltaTime);
      if (ImGui::Button("Reset Economy")) {
        game_data::economy = Economy();
      }
      if (ImGui::InputText("Window Title", settings::windowTitle,
                           IM_ARRAYSIZE(settings::windowTitle))) {
        glfwSetWindowTitle(window, settings::windowTitle);
      }
      ImGui::ColorEdit4("Edit Background Color", &settings::clearColor[0]);
      ImGui::End();
    }
    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(settings::clearColor.x, settings::clearColor.y,
                 settings::clearColor.z, settings::clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
  return cleanup();
}

void initGlfw() {
  for (auto &[k, v] : initialization::GlfwInitFlags) {
    glfwInitHint(k, v);
  }
  if (!glfwInit())
    throw std::runtime_error("Could not initialize GLFW");
  for (auto &[k, v] : initialization::GlfwFlags) {
    glfwWindowHint(k, v);
  }
  window = glfwCreateWindow(settings::width, settings::height,
                            settings::windowTitle, NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("Could not create window");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
}

void initImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  for (auto &v : initialization::ImGuiFlags) {
    io.ConfigFlags |= v;
  }
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
}

int cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
