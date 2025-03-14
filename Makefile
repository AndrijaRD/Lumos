# Compiler and flags
CXX = g++
CXXFLAGS = -fPIC -Wall -Wextra -O2
CXXFLAGS += -fstack-protector-strong -fno-omit-frame-pointer
CXXFLAGS += -std=c++20 -O3 -flto=auto
CXXFLAGS += -Werror -Wpedantic -Wfloat-equal -Wshadow

# Directories
SRC_DIR = lib
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Target library
TARGET_LIB = libLumos.so

# Create build directories
$(BUILD_DIR)/%/:
	mkdir -p $@

# Build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)/%/
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build shared library
$(TARGET_LIB): $(OBJ_FILES)
	$(CXX) -shared -o $@ $^

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET_LIB)
