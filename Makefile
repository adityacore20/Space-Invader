# ==============================
# Compiler
# ==============================
CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# ==============================
# Project
# ==============================
TARGET = app
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

# ==============================
# OS Detection
# ==============================
UNAME_S := $(shell uname -s)

# ==============================
# Raylib flags per OS
# ==============================
ifeq ($(UNAME_S),Darwin)
    # macOS (Homebrew)
    RAYLIB_PATH := $(shell brew --prefix raylib 2>/dev/null)
    CXXFLAGS += -I$(RAYLIB_PATH)/include
    LDFLAGS  += -L$(RAYLIB_PATH)/lib -lraylib \
                -framework OpenGL -framework Cocoa \
                -framework IOKit -framework CoreVideo
endif

ifeq ($(UNAME_S),Linux)
    # Linux
    LDFLAGS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# ==============================
# Rules
# ==============================
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	@echo "✅ Build successful"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild

