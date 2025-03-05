# Compiler and flags
CC = g++
CFLAGS = -Wall -std=c++11

# Default target builds both executables
all: noforward forward

# Build the non-forwarding processor executable
noforward: noforward.cpp
	$(CC) $(CFLAGS) -o noforward noforward.cpp

# Build the forwarding processor executable
forward: forward.cpp
	$(CC) $(CFLAGS) -o forward forward.cpp

# Clean up generated files
clean:
	rm -f noforward forward
