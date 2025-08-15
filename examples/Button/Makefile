# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -g3 -std=c++23 -I../../lib
CXXFLAGS += $(shell pkg-config --cflags SDL3 SDL3_image SDL3_ttf) \
            -isystem $(shell pkg-config --cflags-only-I opencv4 | sed 's/-I//g')


LDFLAGS := $(shell pkg-config --libs SDL3 SDL3_image SDL3_ttf opencv4)
LDFLAGS += -ldl -lpq -ldlib -llapack -lblas -lcblas -lgif

# Directories
SRCDIR := .
BUILDDIR := ../../build/examples/Button
LIBDIR := ../../lib
LIBBUILDDIR := ../../build/lib

# Files
SRC := $(SRCDIR)/Button.cpp
LIB_SRC := $(wildcard $(LIBDIR)/**/*.cpp)

OBJ := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))
LIB_OBJ := $(patsubst $(LIBDIR)/%.cpp, $(LIBBUILDDIR)/%.o, $(LIB_SRC))

# Target
TARGET := Button

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) $(LIB_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIBBUILDDIR)/%.o: $(LIBDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)
	rm -rf $(LIBBUILDDIR)
