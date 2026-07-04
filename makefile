# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -O2 `pkg-config --cflags opencv4`

# Linker flags
LDFLAGS = `pkg-config --libs opencv4`

# Project Files
TARGET = player
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files and create executable
$(TARGET): $(OBJS)
	$(CXX) -o ${TARGET} ${OBJS} ${LDFLAGS}

# Compile into object files
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the files
clean:
	rm -f $(OBJS) $(TARGET)