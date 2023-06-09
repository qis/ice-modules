export PATH := $(ACE)/bin:$(PATH)

PRESETS := linux-shared linux-static mingw-shared mingw-static

all: build

run: build
	build/linux-shared/Debug/main

install: build
	cmake --install build/linux-shared --config Debug

build: configure
	cmake --build build/linux-shared --config Debug

configure: $(addsuffix /build.ninja,$(addprefix build/,$(PRESETS)))

build/%/build.ninja: CMakeLists.txt CMakePresets.json
	cmake --preset "$*"

clean:
	rm -rf build

.PHONY: all run install build configure clean
