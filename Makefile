# Makefile for GemCLI

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm

# Target executables
WATER_TARGET = water_escape
MAGNET_TARGET = magnetic_windows
TERMINAL_TARGET = terminal_water_fire
GAME_TARGET = terminal_game
GEMINI_TARGET = gemini_terminal
INSTALL_DIR ?= $(HOME)/.local/bin

# Source files
WATER_SOURCES = water_escape.c
MAGNET_SOURCES = magnetic_windows.c
TERMINAL_SOURCES = terminal_water_fire.c
GAME_SOURCES = terminal_game.c
GEMINI_SOURCES = gemini_terminal.c config_manager.c utils.c gemini_api.c

# Default target - build GemCLI
all: $(GEMINI_TARGET)

# Build GemCLI (requires curl)
$(GEMINI_TARGET): $(GEMINI_SOURCES)
	$(CC) $(CFLAGS) -o $(GEMINI_TARGET) $(GEMINI_SOURCES)

# Build terminal game (no external dependencies)
$(GAME_TARGET): $(GAME_SOURCES)
	$(CC) $(CFLAGS) -o $(GAME_TARGET) $(GAME_SOURCES) -lm

# Build terminal water fire simulation (no external dependencies)
$(TERMINAL_TARGET): $(TERMINAL_SOURCES)
	$(CC) $(CFLAGS) -o $(TERMINAL_TARGET) $(TERMINAL_SOURCES) -lm

# Build water escape simulation
$(WATER_TARGET): $(WATER_SOURCES)
	$(CC) $(CFLAGS) -o $(WATER_TARGET) $(WATER_SOURCES) $(LIBS)

# Build magnetic windows simulation
$(MAGNET_TARGET): $(MAGNET_SOURCES)
	$(CC) $(CFLAGS) -o $(MAGNET_TARGET) $(MAGNET_SOURCES) $(LIBS)

# Build all programs
all-programs: $(GEMINI_TARGET) $(GAME_TARGET) $(TERMINAL_TARGET) $(WATER_TARGET) $(MAGNET_TARGET)

# Clean build artifacts
clean:
	rm -f $(GEMINI_TARGET) $(GAME_TARGET) $(TERMINAL_TARGET) $(WATER_TARGET) $(MAGNET_TARGET)

# Install GemCLI to INSTALL_DIR as 'gemini'
install: $(GEMINI_TARGET)
	@mkdir -p $(INSTALL_DIR)
	cp $(GEMINI_TARGET) $(INSTALL_DIR)/gemini
	@echo "GemCLI installed as 'gemini' to $(INSTALL_DIR)"
	@echo "Please ensure $(INSTALL_DIR) is in your PATH."
	@echo "You may need to open a new terminal or run: source ~/.bashrc (or ~/.zshrc)"

# Uninstall 'gemini' from INSTALL_DIR
uninstall:
	rm -f $(INSTALL_DIR)/gemini
	@echo "'gemini' uninstalled from $(INSTALL_DIR)"

# Install SDL2 dependencies (Ubuntu/Debian)
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

# Install SDL2 dependencies (macOS with Homebrew)
install-deps-macos:
	brew install sdl2 sdl2_ttf sdl2_image

# Install SDL2 dependencies (Fedora/RHEL)
install-deps-fedora:
	sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_image-devel

# Run GemCLI (example)
run: $(GEMINI_TARGET)
	./$(GEMINI_TARGET) "Hello, how are you?"

# Run the terminal game
run-game: $(GAME_TARGET)
	./$(GAME_TARGET)

# Run the terminal water simulation
run-terminal: $(TERMINAL_TARGET)
	./$(TERMINAL_TARGET)

# Run the water escape simulation
run-water: $(WATER_TARGET)
	./$(WATER_TARGET)

# Run the magnetic windows simulation
run-magnet: $(MAGNET_TARGET)
	./$(MAGNET_TARGET)

# Debug build for water escape
debug: CFLAGS += -g -DDEBUG
debug: $(WATER_TARGET)

# Debug build for magnetic windows
debug-magnet: CFLAGS += -g -DDEBUG
debug-magnet: $(MAGNET_TARGET)

.PHONY: all clean install uninstall install-deps-ubuntu install-deps-macos install-deps-fedora run run-magnet debug debug-magnet both
