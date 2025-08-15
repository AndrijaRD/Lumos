# Compiler and Tools
CXX         = g++
PKG_CONFIG  = pkg-config

# Directories
SRC_DIR     = lib
BUILD_DIR   = build

# -------------------------------------------------------------------
# 1) Make sure make itself sees the right pkg‐config path:
override PKG_CONFIG_PATH := /usr/local/lib/pkgconfig:$(PKG_CONFIG_PATH)

# 2) If your distro names it opencv.pc instead of opencv4.pc, fall back:
PKG_CONFIG ?= pkg-config
OPENCV_PKG := $(shell $(PKG_CONFIG) --exists opencv4 && echo opencv4 || echo opencv)

# 3) Now safely grab CFLAGS & LDFLAGS:
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags  $(OPENCV_PKG) | sed 's/-I/-isystem /g')
OPENCV_LIBS   := $(shell $(PKG_CONFIG) --libs    $(OPENCV_PKG))

# Compiler Flags
CXXFLAGS  = -fPIC -Wall -Wextra \
            -fstack-protector-strong -fno-omit-frame-pointer \
            $(OPENCV_CFLAGS) \
            -std=c++20 -O3 -flto=auto 
            #-Werror -Wpedantic -Wfloat-equal -Wshadow \
            #-std=c++20 -O0 -g -fdiagnostics-color=always

# Library flags
LDFLAGS = -L/usr/local/lib -lLumos $(shell pkg-config --libs Lumos) -lSDL3 -lSDL3_ttf -lSDL3_image -lpq \
          -ldlib -lopencv_dnn -llapack -lblas -lcblas $(OPENCV_LIBS) \
          -lpng -ljxl -lX11 -lgif -ljxl_threads -ljpeg -lwebp `pkg-config --cflags --libs opencv4` 


# Source and Object Files
SRC_FILES = $(wildcard $(SRC_DIR)/*/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Target shared library name
TARGET_LIB = libLumos.so

# Pattern rule: Create build directory for object files
$(BUILD_DIR)/%/:
	mkdir -p $@

# Pattern rule: Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)/%/
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule: Link all object files into a shared library
$(TARGET_LIB): $(OBJ_FILES)
	$(CXX) -shared -o $@ $^ $(OPENCV_LIBS) $(LDFLAGS)

# Clean: Remove build directory and generated library
clean:
	rm -fr $(BUILD_DIR) $(TARGET_LIB)

.PHONY: clean
