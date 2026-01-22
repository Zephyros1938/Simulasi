#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <map>
#include <vector>

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
const int height = width * (aspectx / aspecty);
} // namespace settings

namespace game_data {
class Economy {
public:
  double totalShare;

  void update(double dt) { totalShare += 1 * dt; }
};

static Economy economy;
} // namespace game_data

int main() {
  for (auto &[k, v] : initialization::GlfwInitFlags) {
    glfwInitHint(k, v);
  }
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  for (auto &[k, v] : initialization::GlfwFlags) {
    glfwWindowHint(k, v);
  }

  GLFWwindow *window = glfwCreateWindow(settings::width, settings::height,
                                        settings::windowTitle, NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

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

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    game_data::economy.update(1);

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
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
