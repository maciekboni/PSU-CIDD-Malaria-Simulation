mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
PWD := $(dir $(mkfile_path))

.PHONY: all test clean build

all: build

build:
	cmake --build build --config Release

ci: generate-vcpkg build test

test:
	cmake --build build --target build_and_test

clean:
	rm -rf build

gvc: generate-vcpkg
generate-vcpkg: clean
	if [ ! -x "./ext/vcpkg/vcpkg" ]; then \
		git submodule update --init; \
		./ext/vcpkg/bootstrap-vcpkg.sh; \
  fi

	./ext/vcpkg/vcpkg install gsl
	./ext/vcpkg/vcpkg install yaml-cpp
	./ext/vcpkg/vcpkg install fmt
	./ext/vcpkg/vcpkg install date
	./ext/vcpkg/vcpkg install args
	./ext/vcpkg/vcpkg install CLI11
	./ext/vcpkg/vcpkg install gtest
	./ext/vcpkg/vcpkg install catch
  # install other dependcies here
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .
	cp $(PWD)build/compile_commands.json $(PWD)

g: generate
generate: clean
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
	cp $(PWD)build/compile_commands.json $(PWD)

g0t: generate_without_test

generate_without_test: clean
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_TEST=0 . -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
	cp $(PWD)build/compile_commands.json $(PWD)