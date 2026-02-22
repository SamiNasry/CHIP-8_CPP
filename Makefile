# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./SDL3/include

# Linker settings
LDFLAGS = -L./SDL3/lib -lSDL3

# Directories and files
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = chip8.exe
DLL = SDL3.dll

# Default target now requires both the executable and the DLL
all: $(EXEC) $(DLL)

# Linking the executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Compiling source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# This rule checks if SDL3.dll exists in the root. 
# If it doesn't, it copies it automatically during the build.
$(DLL):
	copy .\SDL3\bin\$(DLL) .\

clean:
	del /Q $(SRC_DIR)\*.o $(EXEC) $(DLL)