############################
# Makefile for PDP Project
############################

CC = mpicc
CCFLAGS = -g -O3 -Wall -Wextra
LIBS = -lm

# List of all source files
SRCS = project.c prop.c

# Generate object file names from source file names
OBJS = $(SRCS:.c=.o)

# Default target
all: mc

# Link object files to generate executable
mc: $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o mc $(LIBS)

# Compile each source file into an object file
%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	$(RM) $(OBJS) mc