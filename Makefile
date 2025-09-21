# Define the executable and source files
exec = thclc.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g -Wall -lm -ldl -fPIC -rdynamic -lX11

# Rule to build the executable
$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

# Rule for compiling object files
%.o: %.c
	gcc -c $(flags) $< -o $@

# Clean rule to remove generated files
clean:
	-rm -f $(exec) $(objects)

