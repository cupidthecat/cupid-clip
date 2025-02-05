# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -O2

# Target executable name
TARGET  = cupid-clip

# Default target: build the executable
all: $(TARGET)

# Compile the source file
$(TARGET): cupid-clip.c
	$(CC) $(CFLAGS) -o $(TARGET) cupid-clip.c

# Clean up build artifacts
clean:
	rm -f $(TARGET)

# Install the executable to /usr/bin
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/bin..."
	install -m 0755 $(TARGET) /usr/bin/$(TARGET)

# Uninstall: remove the executable from /usr/bin
uninstall:
	@echo "Removing /usr/bin/$(TARGET)..."
	rm -f /usr/bin/$(TARGET)
