# ============================
# Toolchain
# ============================

CC      := x86_64-w64-mingw32-gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -g
INCLUDE := -Iinclude

# ============================
# Paths
# ============================

SRC_DIR := src
BIN_DIR := bin
OUT_DIR := output

TARGET  := $(BIN_DIR)/minilang.exe

# ============================
# Sources
# ============================

SRCS := $(wildcard $(SRC_DIR)/*.c)

# ============================
# Build rules
# ============================

all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) -o $@

$(BIN_DIR):
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)

# ============================
# Utility targets
# ============================

run: $(TARGET)
	$(TARGET)

clean:
	del /Q $(BIN_DIR)\*.exe
	del /Q $(OUT_DIR)\*.dot
	del /Q $(OUT_DIR)\*.png

.PHONY: all run clean
