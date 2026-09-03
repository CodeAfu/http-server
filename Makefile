BUILD_DIR := build
TARGET := http-server

.PHONY: all build run clean configure

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -G Ninja

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
