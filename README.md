# PersonalFinanceManager

## Prebuilt (Windows)
- Download `dist/PersonalFinanceManager-Windows.zip` from the repository (or Releases).
- Extract and run `PersonalFinanceManager-Windows.exe`.
- Ensure `raylib.dll` and `glfw3.dll` are in the same folder as the `.exe` (both bundled in the prebuild ZIP).
- Requires Microsoft Visual C++ Redistributable (2015–2022). If missing, install from https://aka.ms/vs/17/release/vc_redist.x64.exe.

## Prebuilt (macOS)
- Download the prebuilt executable from `prebuild/PersonalFinanceManager` in the repository.
- Make it executable: `chmod +x PersonalFinanceManager`.
- Run: `./PersonalFinanceManager`.
- If blocked by Gatekeeper, allow under System Settings → Privacy & Security → "Open Anyway".

## Build from Source (Windows)
- Prereqs: Git, Visual Studio 2022 C++ workload, CMake.
- Clone and checkout branch: `git clone https://github.com/simble2k/PersonalFinanceManager.git && cd PersonalFinanceManager && git checkout myGUI`.
- Configure: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake`.
- Build: `cmake --build build --config Debug`.

## Build from Source (macOS)
- Clone and checkout branch: `git clone https://github.com/simble2k/PersonalFinanceManager.git && cd PersonalFinanceManager && git checkout myGUI`.
- Install Xcode Command Line Tools: `xcode-select --install`.
- Install dependencies: Requires raylib and other dependencies via your preferred package manager.
- Configure (Intel): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=x86_64`.
- Configure (Apple Silicon): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=arm64`.
- Build: `cmake --build build --config Debug`.
- Run: `./build/PersonalFinanceManager`.