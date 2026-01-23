#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <functionlang.hpp>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <vector>

namespace utilities {
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

} // namespace utilities
namespace initialization {
using namespace std;

vector<uint32_t> ImGuiFlags = {ImGuiConfigFlags_DockingEnable,
                               ImGuiConfigFlags_ViewportsEnable,
                               ImGuiConfigFlags_IsSRGB};
map<uint32_t, uint32_t> GlfwFlags = {
    {GLFW_CONTEXT_VERSION_MAJOR, 4},
    {GLFW_CONTEXT_VERSION_MINOR, 6},
    {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE},
    {GLFW_CONTEXT_DEBUG, GLFW_TRUE},
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

template <typename T, T DefaultValue, int HistoryLength> class EconomyObject {
public:
  EconomyObject(float baseLevel = 1.0f, const char *upgradeLevelData = nullptr,
                const char *valueIncreaseData = nullptr,
                std::string name = uuid::generate_uuid_v4())
      : value(DefaultValue), level(baseLevel), minValue(DefaultValue),
        maxValue(DefaultValue), name(name)
  // Initialize directly to the lambda
  {
    std::fill_n(history, HistoryLength, DefaultValue);
    if (upgradeLevelData != nullptr) {
      upgradeLevelFormula = LogicEvaluator(upgradeLevelData);
    } else {
      upgradeLevelFormula = LogicEvaluator("*10,^1.15,V0");
    }
    if (valueIncreaseData != nullptr) {
      rateIncreaseFormula = LogicEvaluator(valueIncreaseData);
    } else {
      rateIncreaseFormula = LogicEvaluator("+V0,V1");
    }
  }

  void update(float dt) {
    value = rateIncreaseFormula.evaluate({value, level * dt});
    utilities::pushToBackOfArray(history, value);
    minValue = utilities::minElement(history);
    maxValue = utilities::maxElement(history);
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
  LogicEvaluator upgradeLevelFormula;
  LogicEvaluator rateIncreaseFormula;
  std::string name;
};

template <float DefaultValue = 0, int HistoryLength = 64>
class Stock : public EconomyObject<float, DefaultValue, HistoryLength> {};

class Economy {
public:
  EconomyObject<float, 10.0f, 1024> baseShare;
  void update(double dt) { baseShare.update(dt); }
};
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
        game_data::economy = game_data::Economy();
      }
      if (ImGui::InputText("Window Title", settings::windowTitle,
                           IM_ARRAYSIZE(settings::windowTitle))) {
        glfwSetWindowTitle(window, settings::windowTitle);
      }
      ImGui::ColorEdit4("Edit Background Color", &settings::clearColor[0]);
      ImGui::End();
    }

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
