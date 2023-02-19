#Example run
#make PROGRAM=create_cpp
PROGRAM = create_cpp

BUILD_DIR = bin
OUT_DIR = out # Temporary tools output
CXX = g++
CPPFLAGS += -Wall -Wformat -Os
LDFLAGS =
EXE =
CXXFLAGS =

OBJS = src/main.cpp

ifeq ($(PROGRAM), create_cpp)
OBJS += src/create_cpp.cpp
DEFINES += -DPROGRAM_CREATE_CPP
EXE = create_cpp.out
endif
ifeq ($(PROGRAM), hello)
OBJS += src/hello.cpp
DEFINES += -DPROGRAM_HELLO
EXE = hello.out
endif

all: $(BUILD_DIR)/$(EXE)

$(BUILD_DIR)/%.o:%.cpp
	$(CXX) --std=c++11 $(CPPFLAGS) $(CXXFLAGS) -c $(DEFINES) -o $@ $<

$(BUILD_DIR)/$(EXE):  $(OBJS) $(BUILD_DIR) 
	$(CXX) --std=c++11 $(DEFINES) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)
