mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
PWD := $(dir $(mkfile_path))

.PHONY: all test clean build

all: generate-vcpkg build

build:
	cmake --build build --config Release -- -j4

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

	./ext/vcpkg/vcpkg install gsl yaml-cpp fmt date args CLI11 gtest catch
  # install other dependcies here
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release  .
	cp $(PWD)build/compile_commands.json $(PWD)

g: generate
generate: clean
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake .
	cp $(PWD)build/compile_commands.json $(PWD)

reload_cmake:
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 . -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
	cp $(PWD)build/compile_commands.json $(PWD)

g0t: generate_without_test

generate_without_test: clean
	cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_TEST=0 . -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
	cp $(PWD)build/compile_commands.json $(PWD)
