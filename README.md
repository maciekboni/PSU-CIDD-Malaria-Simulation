# PSU-CIDD-Malaria-Simulation
![CI](https://github.com/maciekboni/PSU-CIDD-Malaria-Simulation/actions/workflows/ci.yml/badge.svg)

[Penn State](https://www.psu.edu/) - [Center for Infectious Disease Dynamics (CIDD)](https://www.huck.psu.edu/institutes-and-centers/center-for-infectious-disease-dynamics) - [Boni Lab](http://mol.ax/)

---
## Build

Checkout / update the 'vcpkg' external project
```bash
git submodule update --init --recursive
```

Bootstrap `vcpkg` and install dependencies
```bash
./ext/vcpkg/bootstrap-vcpkg.sh # ./ext/vcpkg/bootstrap-vcpkg.bat
./ext/vcpkg/vcpkg install gsl
./ext/vcpkg/vcpkg install yaml-cpp
./ext/vcpkg/vcpkg install fmt
./ext/vcpkg/vcpkg install date
./ext/vcpkg/vcpkg install args
./ext/vcpkg/vcpkg install CLI11
./ext/vcpkg/vcpkg install gtest
./ext/vcpkg/vcpkg install catch

```
Build
```bash
cmake --build build --config Release
```

---

## About

---

## Documentation
