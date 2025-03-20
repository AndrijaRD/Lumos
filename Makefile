# Compiler and Tools
CXX         = g++
PKG_CONFIG  = pkg-config

# Directories
SRC_DIR     = lib
BUILD_DIR   = build

# OpenCV flags from pkg-config
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags opencv4 | sed 's/-I/-isystem /g')
OPENCV_LIBS   := $(shell $(PKG_CONFIG) --libs opencv4)

# Compiler Flags
CXXFLAGS  = -fPIC -Wall -Wextra \
            -fstack-protector-strong -fno-omit-frame-pointer \
            -std=c++20 -O3 -flto=auto \
            #-Werror -Wpedantic -Wfloat-equal -Wshadow \
            $(OPENCV_CFLAGS)

# Library flags
LDFLAGS = -L/usr/local/lib -lLumos $(shell pkg-config --libs Lumos) -lSDL2 -lSDL2_ttf -lSDL2_image -lpq \
          -ldlib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_dnn -llapack -lblas -lcblas \
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
	rm -rf $(BUILD_DIR) $(TARGET_LIB)

.PHONY: clean
