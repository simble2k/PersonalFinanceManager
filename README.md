# PersonalFinanceManager

## Prebuilt (macOS)
- Download the prebuilt executable from `prebuild/PersonalFinanceManager` in the repository.
- Make it executable: `chmod +x PersonalFinanceManager`.
- Run: `./PersonalFinanceManager`.
- If blocked by Gatekeeper, allow under System Settings → Privacy & Security → "Open Anyway".

## Build from Source (macOS)
- Clone and checkout branch: `git clone https://github.com/simble2k/PersonalFinanceManager.git && cd PersonalFinanceManager && git checkout myGUI`.
- Install Xcode Command Line Tools: `xcode-select --install`.
- Install dependencies: Requires raylib and other dependencies via your preferred package manager.
- Configure (Intel): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=x86_64`.
- Configure (Apple Silicon): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_ARCHITECTURES=arm64`.
- Build: `cmake --build build --config Debug`.
- Run: `./build/PersonalFinanceManager`.