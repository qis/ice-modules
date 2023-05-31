export PATH := $(ACE)/bin:$(PATH)
export WINEPATH := $(ACE)/sys/mingw/bin

PRESETS := linux-shared linux-static mingw-shared mingw-static

all: build

run: build/linux-shared/build.ninja
	@cmake --build build/linux-shared --config Debug --target main
	@build/linux-shared/Debug/main

main: build
	build/linux-shared/Debug/main
	build/linux-shared/Release/main
	build/linux-shared/MinSizeRel/main
	build/linux-shared/RelWithDebInfo/main
	build/linux-shared/Coverage/main
	build/linux-static/Debug/main
	build/linux-static/Release/main
	build/linux-static/MinSizeRel/main
	build/linux-static/RelWithDebInfo/main
	build/linux-static/Coverage/main
	wine build/mingw-shared/Debug/main.exe
	wine build/mingw-shared/Release/main.exe
	wine build/mingw-shared/MinSizeRel/main.exe
	wine build/mingw-shared/RelWithDebInfo/main.exe
	wine build/mingw-shared/Coverage/main.exe
	wine build/mingw-static/Debug/main.exe
	wine build/mingw-static/Release/main.exe
	wine build/mingw-static/MinSizeRel/main.exe
	wine build/mingw-static/RelWithDebInfo/main.exe
	wine build/mingw-static/Coverage/main.exe

install: configure
	@cmake --build build/linux-shared --config Debug --target install
	@cmake --build build/linux-shared --config Release --target install
	@cmake --build build/linux-shared --config MinSizeRel --target install
	@cmake --build build/linux-shared --config RelWithDebInfo --target install
	@cmake --build build/linux-shared --config Coverage --target install
	@cmake --build build/linux-static --config Debug --target install
	@cmake --build build/linux-static --config Release --target install
	@cmake --build build/linux-static --config MinSizeRel --target install
	@cmake --build build/linux-static --config RelWithDebInfo --target install
	@cmake --build build/linux-static --config Coverage --target install
	@cmake --build build/mingw-shared --config Debug --target install
	@cmake --build build/mingw-shared --config Release --target install
	@cmake --build build/mingw-shared --config MinSizeRel --target install
	@cmake --build build/mingw-shared --config RelWithDebInfo --target install
	@cmake --build build/mingw-shared --config Coverage --target install
	@cmake --build build/mingw-static --config Debug --target install
	@cmake --build build/mingw-static --config Release --target install
	@cmake --build build/mingw-static --config MinSizeRel --target install
	@cmake --build build/mingw-static --config RelWithDebInfo --target install
	@cmake --build build/mingw-static --config Coverage --target install

build: configure
	@cmake --build build/linux-shared --config Debug
	@cmake --build build/linux-shared --config Release
	@cmake --build build/linux-shared --config MinSizeRel
	@cmake --build build/linux-shared --config RelWithDebInfo
	@cmake --build build/linux-shared --config Coverage
	@cmake --build build/linux-static --config Debug
	@cmake --build build/linux-static --config Release
	@cmake --build build/linux-static --config MinSizeRel
	@cmake --build build/linux-static --config RelWithDebInfo
	@cmake --build build/linux-static --config Coverage
	@cmake --build build/mingw-shared --config Debug
	@cmake --build build/mingw-shared --config Release
	@cmake --build build/mingw-shared --config MinSizeRel
	@cmake --build build/mingw-shared --config RelWithDebInfo
	@cmake --build build/mingw-shared --config Coverage
	@cmake --build build/mingw-static --config Debug
	@cmake --build build/mingw-static --config Release
	@cmake --build build/mingw-static --config MinSizeRel
	@cmake --build build/mingw-static --config RelWithDebInfo
	@cmake --build build/mingw-static --config Coverage

configure: $(addsuffix /build.ninja,$(addprefix build/,$(PRESETS)))

build/%/build.ninja: CMakeLists.txt CMakePresets.json
	@cmake --preset "$*"

clean:
	@rm -f default.profraw
	@rm -rf build

reset: clean
	@rm -rf modules

.PHONY: all run main install build configure clean reset
