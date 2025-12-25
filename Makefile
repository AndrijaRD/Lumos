# High-Quality Makefile for Building and Installing Lumos Library
# - Supports parallel compilation (-j).
# - Automatic dependency tracking (.d files).
# - Handles subdirectories in src.
# - Debug/Release builds (make DEBUG=1 for debug).
# - Install/uninstall targets (requires sudo for system install).
# - Clean and rebuild.
# - Uses modern practices: phony targets, order-only prerequisites, silent mode option.

# Project Settings
LIB_NAME := libLumos.so
SRC_DIR := lib
BUILD_DIR := build
INSTALL_LIB_DIR := /usr/local/lib
INSTALL_INCLUDE_DIR := /usr/local/include/Lumos
PKGCONFIG_DIR := /usr/lib/pkgconfig
PKGCONFIG_FILE := Lumos.pc

# Compiler and Flags
CXX := g++
CXXFLAGS := -fPIC -Wall -Wextra -fstack-protector-strong -fno-omit-frame-pointer -std=c++20 -O3 -flto=auto
LDFLAGS := -shared -L/usr/local/lib -lSDL3 -lSDL3_ttf -lSDL3_image -lpq -ldlib -lopencv_dnn -llapack -lblas -lcblas -lpng -ljxl -lX11 -lgif -ljxl_threads -ljpeg -lwebp

# Dynamic flags from pkg-config
PKG_CONFIG ?= pkg-config
OPENCV_PKG := $(shell $(PKG_CONFIG) --exists opencv4 && echo opencv4 || echo opencv)
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(OPENCV_PKG) | sed 's/-I/-isystem /g')
OPENCV_LIBS := $(shell $(PKG_CONFIG) --libs $(OPENCV_PKG))

PANGO_PKG := pangocairo
PANGO_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(PANGO_PKG) fontconfig freetype2 harfbuzz | sed 's/-I/-isystem /g')
PANGO_LIBS := $(shell $(PKG_CONFIG) --libs $(PANGO_PKG) fontconfig freetype2 harfbuzz)

CXXFLAGS += $(OPENCV_CFLAGS) $(PANGO_CFLAGS)

LDFLAGS += $(OPENCV_LIBS) $(PANGO_LIBS)

# Debug mode
ifdef DEBUG
CXXFLAGS += -g -O0 -DDEBUG
endif

# Source and Object Files
SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP_FILES := $(OBJ_FILES:.o=.d)

# Build Rules
all: $(LIB_NAME)

$(LIB_NAME): $(OBJ_FILES)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

# Install
install: $(LIB_NAME)
	@sudo mkdir -p $(INSTALL_LIB_DIR) $(INSTALL_INCLUDE_DIR) $(PKGCONFIG_DIR)
	@sudo cp $(LIB_NAME) $(INSTALL_LIB_DIR)
	@sudo cp -r $(SRC_DIR)/* $(INSTALL_INCLUDE_DIR)
	@echo "prefix=/usr/local\nexec_prefix=\$${prefix}\nlibdir=\$${exec_prefix}/lib\nincludedir=\$${prefix}/include/Lumos\n\nName: Lumos\nDescription: A custom library for various utilities\nVersion: 2.0.0\nLibs: -L\$${libdir} -lLumos\nCflags: -I\$${includedir}" | sudo tee $(PKGCONFIG_DIR)/$(PKGCONFIG_FILE) > /dev/null
	@sudo ldconfig
	@echo "Lumos installed successfully."

# Uninstall
uninstall:
	@sudo rm -f $(INSTALL_LIB_DIR)/$(LIB_NAME)
	@sudo rm -rf $(INSTALL_INCLUDE_DIR)
	@sudo rm -f $(PKGCONFIG_DIR)/$(PKGCONFIG_FILE)
	@sudo ldconfig
	@echo "Lumos uninstalled successfully."

# Clean
clean:
	rm -rf $(BUILD_DIR) $(LIB_NAME) $(DEP_FILES)

# Rebuild
rebuild: clean all

# Include dependencies
-include $(DEP_FILES)

.PHONY: all install uninstall clean rebuild



# # Compiler and Tools
# CXX         = g++
# PKG_CONFIG  = pkg-config

# # Directories
# SRC_DIR     = lib
# BUILD_DIR   = build

# # -------------------------------------------------------------------
# # 1) Make sure make itself sees the right pkg‐config path:
# override PKG_CONFIG_PATH := /usr/local/lib/pkgconfig:$(PKG_CONFIG_PATH)

# # 2) If your distro names it opencv.pc instead of opencv4.pc, fall back:
# PKG_CONFIG ?= pkg-config
# OPENCV_PKG := $(shell $(PKG_CONFIG) --exists opencv4 && echo opencv4 || echo opencv)

# # 3) Now safely grab CFLAGS & LDFLAGS:
# OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags  $(OPENCV_PKG) | sed 's/-I/-isystem /g')
# OPENCV_LIBS   := $(shell $(PKG_CONFIG) --libs    $(OPENCV_PKG))

# # 4) Add Pango/Cairo/Fontconfig/FreeType/HarfBuzz flags
# PANGO_PKG := pangocairo
# PANGO_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(PANGO_PKG) fontconfig freetype2 harfbuzz | sed 's/-I/-isystem /g')
# PANGO_LIBS   := $(shell $(PKG_CONFIG) --libs   $(PANGO_PKG) fontconfig freetype2 harfbuzz)

# # Compiler Flags
# CXXFLAGS  = -fPIC -Wall -Wextra \
#             -fstack-protector-strong -fno-omit-frame-pointer \
#             $(OPENCV_CFLAGS) \
#             $(PANGO_CFLAGS) \
#             -std=c++20 -O3 -flto=auto 
#             #-Werror -Wpedantic -Wfloat-equal -Wshadow \
#             #-std=c++20 -O0 -g -fdiagnostics-color=always

# # Library flags (linker flags)
# LDFLAGS = -L/usr/local/lib -lSDL3 -lSDL3_ttf -lSDL3_image -lpq \
#           -ldlib -lopencv_dnn -llapack -lblas -lcblas $(OPENCV_LIBS) \
#           -lpng -ljxl -lX11 -lgif -ljxl_threads -ljpeg -lwebp \
#           $(PANGO_LIBS)

# # Source and Object Files
# SRC_FILES = $(wildcard $(SRC_DIR)/*/*.cpp)
# OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# # Target shared library name
# TARGET_LIB = libLumos.so

# # Pattern rule: Create build directory for object files
# $(BUILD_DIR)/%/:
# 	mkdir -p $@

# # Pattern rule: Compile source files into object files
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)/%/
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Rule: Link all object files into a shared library
# $(TARGET_LIB): $(OBJ_FILES)
# 	$(CXX) -shared -o $@ $^ $(LDFLAGS)

# # Clean: Remove build directory and generated library
# clean:
# 	rm -fr $(BUILD_DIR) $(TARGET_LIB)

# .PHONY: clean
