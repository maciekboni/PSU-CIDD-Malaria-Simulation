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
./ext/vcpkg/vcpkg install gsl yaml-cpp fmt date args CLI11 gtest catch

```
Build
```bash
cmake --build build --config Release
```

---

## About

---

## Documentation
