# PersonalFinanceManager

## Prebuilt (Windows)
- Download `dist/PersonalFinanceManager-Windows.zip` from the repository (or Releases).
- Extract and run `PersonalFinanceManager-Windows.exe`.
- Requires Microsoft Visual C++ Redistributable (2015–2022). If missing, install from https://aka.ms/vs/17/release/vc_redist.x64.exe.

## Build from Source (Windows)
- Prereqs: Git, Visual Studio 2022 C++ workload, CMake.
- Clone and checkout branch: `git clone https://github.com/simble2k/PersonalFinanceManager.git && cd PersonalFinanceManager && git checkout myGUI`.
- Configure: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake`.
- Build: `cmake --build build --config Debug`.