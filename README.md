# PersonalFinanceManager

## Prebuilt (Windows)
- Download `dist/PersonalFinanceManager-Windows.zip` from the repository (or Releases).
- Extract and run `PersonalFinanceManager-Windows.exe`.
- Requires Microsoft Visual C++ Redistributable (2015–2022). If missing, install from https://aka.ms/vs/17/release/vc_redist.x64.exe.

## Prebuilt (macOS)
- On branch `myGUI`, GitHub Actions builds a macOS x64 artifact.
- Go to GitHub → Actions → "macOS Build" → latest run → Artifacts → download `PersonalFinanceManager-mac-x64.zip`.
- Extract and run `PersonalFinanceManager-mac-x64`.
- If blocked by Gatekeeper, allow under System Settings → Privacy & Security → "Open Anyway".

## Build from Source (Windows)
- Prereqs: Git, Visual Studio 2022 C++ workload, CMake.
- Clone and checkout branch: `git clone https://github.com/simble2k/PersonalFinanceManager.git && cd PersonalFinanceManager && git checkout myGUI`.
- Configure: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake`.
- Build: `cmake --build build --config Debug`.

## Build from Source (macOS)
- Install Xcode Command Line Tools: `xcode-select --install`.
- Bootstrap vcpkg: `./vcpkg/bootstrap-vcpkg.sh`.
- Configure (Intel): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-osx -DCMAKE_OSX_ARCHITECTURES=x86_64`.
- Configure (Apple Silicon): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=arm64-osx -DCMAKE_OSX_ARCHITECTURES=arm64`.
- Build: `cmake --build build --config Debug`.