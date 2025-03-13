# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Source files
SRCS = i2ceeprom.cpp

# Executable name
TARGET = i2ceeprom

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Rule to clean up build files
clean:
	rm -f $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean
