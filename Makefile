CXX = g++

# 1. Paths
IMGUI_DIR = ./external/imgui
SRC_DIR = src

# 2. Dynamic Library Detection (The Fix)
# This tries to find glfw3 first, then falls back to glfw
GLFW_PKG = $(shell pkg-config --exists glfw3 && echo glfw3 || echo glfw)
GLFW_CFLAGS = $(shell pkg-config --cflags $(GLFW_PKG))
GLFW_LIBS   = $(shell pkg-config --libs $(GLFW_PKG))

# 3. Include Directories
INCLUDES = -I./external -I./include -I./$(SRC_DIR) \
           -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends \
           $(GLFW_CFLAGS)

# 4. Source Files
SRCS = $(SRC_DIR)/main.cpp \
       $(IMGUI_DIR)/imgui.cpp \
       $(IMGUI_DIR)/imgui_draw.cpp \
       $(IMGUI_DIR)/imgui_widgets.cpp \
       $(IMGUI_DIR)/imgui_tables.cpp \
       $(IMGUI_DIR)/imgui_demo.cpp \
       $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
       $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# 5. Compiler & Linker Flags
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra $(INCLUDES) -MP -MMD
# Added -lGL and -ldl via standard names, combined with pkg-config results
LDFLAGS = $(GLFW_LIBS) -lGL -ldl -lpthread

# 6. Objects & Dependencies
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = app.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
