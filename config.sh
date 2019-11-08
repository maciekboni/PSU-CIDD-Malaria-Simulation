#!/usr/bin/env bash

function check_version() { test "$(printf '%s\n' "$@" | sort -V | head -n 1)" != "$1"; }

# Define some paths
BUILD_ENV="~/work/build_env"
HOME_BUILD_ENV="$HOME/work/build_env"

# Load gcc
module load gcc/7.3.1

# Create the directory to work in
source=$(pwd)
mkdir -p $BUILD_ENV

# If the version of git is to old, load a newer one
required=2.24.0
current=$(git --version | sed "s/git version //g")
if check_version $required $current; then
  cd $BUILD_ENV
  git clone git://git.kernel.org/pub/scm/git/git.git
  cd git
  make configure
  ./configure --prefix=/home
  make all
  export PATH="`pwd`:$PATH"
fi

# If the version of cmake is too old, load a newer one
required="3.15"
current=$(cmake --version | sed "s/cmake version //g")
if check_version $required $current; then
  cd $BUILD_ENV
  wget https://cmake.org/files/v3.15/cmake-3.15.0-Linux-x86_64.tar.gz
  tar xf cmake-3.15.0-Linux-x86_64.tar.gz
  rm cmake-3.15.0-Linux-x86_64.tar.gz
  export PATH="`pwd`/cmake-3.15.0-Linux-x86_64/bin/:$PATH"
fi

# If PostgreSQL isn't present, download and install it to the user directory
if [ ! -d "$BUILD_ENV/postgres" ]; then
  # Get the source files
  cd $BUILD_ENV
  wget https://github.com/postgres/postgres/archive/master.zip
  unzip master.zip
  rm master.zip

  # Build the source files
  cd postgres-master
  ./configure --prefix=$HOME_BUILD_ENV/postgres
  make -j 8
  make install
  cd ..
  rm -rf $BUILD_ENV/postgres-master

  # Export the relevent variables  
  export PATH="$HOME_BUILD_ENV/postgres/bin:$PATH"
  export PKG_CONFIG_PATH="$HOME_BUILD_ENV/postgres/lib/pkgconfig:$PKG_CONFIG_PATH"
  export LIBRARY_PATH="$HOME_BUILD_ENV/postgres/lib:$LIBRARY_PATH"

  # Prepare the libpqxx library
  git clone https://github.com/jtv/libpqxx.git
  cd libpqxx
  ./configure --disable-documentation --prefix=$HOME_BUILD_ENV/lib
  make -j 8
  make install
fi

# If vcpkg doesn't already exist as a directory, load it
if [ ! -d "$BUILD_ENV/vcpkg" ]; then
  cd $BUILD_ENV
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
fi

# Load the relevent packages
cd $BUILD_ENV/vcpkg
./vcpkg install yaml-cpp fmt date args

# Return to the source directory
cd $source

# Create the build script
if [ ! -d "build" ]; then
  mkdir -p build
  cd build
  toolchain="$HOME_BUILD_ENV/vcpkg/scripts/buildsystems/vcpkg.cmake"
  echo "module load gcc/7.3.1" > build.sh
  echo "export PATH=$PATH" >> build.sh
  echo "cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$toolchain .." >> build.sh
  echo "make -j 8" >> build.sh
  chmod +x build.sh
fi
