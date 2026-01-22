#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
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
map<uint32_t, uint32_t> GlfwInitFlags{{GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND}};
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
class Economy {
public:
  float totalShare = 0, level = 0, maxLevel = 0, maxShare = 0, minLevel = 0,
        minShare = 0;
  float levelHistory[255];
  float shareHistory[1024];
  void update(double dt) {
    totalShare += level * dt;
    utilities::pushToBackOfArray(levelHistory, level);
    utilities::pushToBackOfArray(shareHistory, totalShare);
    minLevel = utilities::minElement(levelHistory);
    maxLevel = utilities::maxElement(levelHistory);
    minShare = utilities::minElement(shareHistory);
    maxShare = utilities::maxElement(shareHistory);
  }
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

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
    game_data::economy.update(deltaTime);

    {
      ImGui::Begin("Main Menu");
      ImGui::SeparatorText("General");
      if (ImGui::Button("Quit")) {
        glfwSetWindowShouldClose(window, true);
      }
      if (ImGui::InputText("Window Title", settings::windowTitle,
                           IM_ARRAYSIZE(settings::windowTitle))) {
        glfwSetWindowTitle(window, settings::windowTitle);
      }
      ImGui::ColorEdit4("Edit Background Color", &settings::clearColor[0]);
      ImGui::End();
    }
    {
      ImGui::Begin("Economy");
      ImGui::SeparatorText("Economy");
      ImGui::Text("Money: %f", game_data::economy.totalShare);
      ImGui::Text("Level: %.2f", game_data::economy.level);
      if (ImGui::Button("Level Up")) {
        game_data::economy.level++;
      }
      if (ImGui::Button("Level Down")) {
        game_data::economy.level--;
      }
      {
        ImGui::PlotLines("Level History", game_data::economy.levelHistory, 255,
                         0, "Level", game_data::economy.minLevel,
                         game_data::economy.maxLevel, ImVec2(0, 80));
        if (ImGui::BeginTable("Level Info", 2,
                              ImGuiTableFlags_Borders |
                                  ImGuiTableFlags_RowBg)) {
          ImGui::TableSetupColumn("Min");
          ImGui::TableSetupColumn("Max");
          ImGui::TableHeadersRow();
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("%.2f", game_data::economy.minLevel);
          ImGui::TableNextColumn();
          ImGui::Text("%.2f", game_data::economy.maxLevel);
          ImGui::EndTable();
        }
      }
      {
        ImGui::PlotLines("Share History", game_data::economy.shareHistory, 1024,
                         0, "Share", game_data::economy.minShare,
                         game_data::economy.maxShare, ImVec2(0, 80));
        if (ImGui::BeginTable("Share Info", 2,
                              ImGuiTableFlags_Borders |
                                  ImGuiTableFlags_RowBg)) {
          ImGui::TableSetupColumn("Min");
          ImGui::TableSetupColumn("Max");
          ImGui::TableHeadersRow();
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("%.2f", game_data::economy.minShare);
          ImGui::TableNextColumn();
          ImGui::Text("%.2f", game_data::economy.maxShare);
          ImGui::EndTable();
        }
      }
      ImGui::End();
    }
    {
      ImGui::Begin("Debug");
      ImGui::SeparatorText("Debug");
      ImGui::Text("dt: %.2f", 1.0f / deltaTime);
      if (ImGui::Button("Reset Economy")) {
        game_data::economy = game_data::Economy();
      }
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
