#!/usr/bin/env bash
git pull
mkdir -p build
cd build

export PATH=/usr/lpp/mmfs/bin:/usr/local/bin:/usr/local/sbin:/usr/lib64/qt-3.3/bin:/opt/moab/bin:/opt/mam/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/opt/ibutils/bin:/opt/puppetlabs/bin:/storage/home/#username#/bin:/storage/home/#username#/work/build_env/git
export LIBRARY_PATH=/storage/home/#username#/work/build_env/postgres/lib:/storage/home/#username#/work/build_env/lib/lib:

module load gcc/8.3.1
module load cmake/3.18.4
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/storage/home/#username#/work/build_env/vcpkg/scripts/buildsystems/vcpkg.cmake ..
make MaSim -j 8
