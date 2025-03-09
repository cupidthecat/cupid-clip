# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -O2

# Target executable name
TARGET  = cupid-clip

# Source file path
SRC     = src/cupid-clip.c

# Library directory and name
LIBDIR  = lib
LIBNAME = cupidconf
LIBFILE = $(LIBDIR)/lib$(LIBNAME).a

# Include library directory and link the library
LDFLAGS = -L$(LIBDIR) -l$(LIBNAME)

# Default target: build the executable
all: $(LIBFILE) $(TARGET)

# Build the library
$(LIBFILE): $(LIBDIR)/cupidconf.c
	@echo "Building library..."
	$(CC) $(CFLAGS) -c $(LIBDIR)/cupidconf.c -o $(LIBDIR)/cupidconf.o
	ar rcs $(LIBFILE) $(LIBDIR)/cupidconf.o

# Compile the source file and link the library
$(TARGET): $(SRC) $(LIBFILE)
	@echo "Building executable..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(LIBDIR)/*.o $(LIBFILE)

# Install the executable to /usr/bin
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/bin..."
	install -m 0755 $(TARGET) /usr/bin/$(TARGET)

# Uninstall: remove the executable from /usr/bin
uninstall:
	@echo "Removing /usr/bin/$(TARGET)..."
	rm -f /usr/bin/$(TARGET)
