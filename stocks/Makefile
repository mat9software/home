#
# Makefile to use with emscripten
# See https://emscripten.org/docs/getting_started/downloads.html
# for installation instructions.
#
# This Makefile assumes you have loaded emscripten's environment.
# (On Windows, you may need to execute emsdk_env.bat or encmdprompt.bat ahead)
#
# Running `make` will produce three files:
#  - web/index.html
#  - web/index.js
#  - web/index.wasm
#
# All three are needed to run the demo.

CC = emcc
CXX = em++
WEB_DIR = web
EXE = $(WEB_DIR)/index.html
COMMON_DIR = ../
SRC_DIR = src/
IMGUI_DIR = imgui/
RAPID_JSON_DIR = rapidjson/include
BUILD_DIR = build
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/dockspace.cpp $(SRC_DIR)/fetch.cpp $(SRC_DIR)/ui.cpp $(SRC_DIR)/parse.cpp $(SRC_DIR)/data.cpp $(SRC_DIR)/utils.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)
CPPFLAGS =
LDFLAGS =
EMS =
DEFINES = -DDEBUG_TRACE -DDEBUG_REQUEST

##---------------------------------------------------------------------
## EMSCRIPTEN OPTIONS
##---------------------------------------------------------------------

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
EMS += -s USE_SDL=2
EMS += -s DISABLE_EXCEPTION_CATCHING=1
LDFLAGS += -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -sFETCH

# Uncomment next line to fix possible rendering bugs with Emscripten version older then 1.39.0 (https://github.com/ocornut/imgui/issues/2877)
#EMS += -s BINARYEN_TRAP_MODE=clamp
#EMS += -s SAFE_HEAP=1    ## Adds overhead

# Emscripten allows preloading a file or folder to be accessible at runtime.
# The Makefile for this example project suggests embedding the misc/fonts/ folder into our application, it will then be accessible as "/fonts"
# See documentation for more details: https://emscripten.org/docs/porting/files/packaging_files.html
# (Default value is 0. Set to 1 to enable file-system and include the misc/fonts/ folder as part of the build.)
USE_FILE_SYSTEM ?= 0
ifeq ($(USE_FILE_SYSTEM), 0)
LDFLAGS += -s NO_FILESYSTEM=1
CPPFLAGS += -DIMGUI_DISABLE_FILE_FUNCTIONS
endif
ifeq ($(USE_FILE_SYSTEM), 1)
LDFLAGS += --no-heap-copy --preload-file ../../misc/fonts@/fonts
endif

##---------------------------------------------------------------------
## FINAL BUILD FLAGS
##---------------------------------------------------------------------

CPPFLAGS += -I$(COMMON_DIR) -I$(SRC_DIR) -I$(RAPID_JSON_DIR) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
#CPPFLAGS += -g
CPPFLAGS += -Wall -Wformat -Os $(EMS) $(DEFINES)
LDFLAGS += --shell-file shell_minimal.html $(EMS) $(DEFINES)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------


$(BUILD_DIR)/%.o:$(SRC_DIR)%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

all: $(BUILD_DIR) $(EXE)
	@echo Build complete for $(EXE)

$(BUILD_DIR):
	mkdir $@

$(WEB_DIR):
	mkdir $@

$(EXE): $(OBJS) $(WEB_DIR)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(WEB_DIR) $(BUILD_DIR)
