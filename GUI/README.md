# GUI Overview — PersonalFinanceManager

This README documents the GUI module: what each component does and how to run the prebuilt binary (macOS) or build/run from source.

## Purpose
The GUI provides a windowed interface (built with raylib) for viewing wallets, adding income/expenses, managing recurring transactions, and editing master data (wallets, categories, sources).

## Key components and roles
- `mainGUI.cpp`
  - Program entry for the GUI target. Initializes raylib, creates the `App` controller and runs the main loop.

- `App` (`App.h` / `App.cpp`)
  - Central controller. Loads data via `DataManager`, manages active screen (Dashboard, Transaction, Recurring, Master Data), forwards `Update()`/`Draw()` and saves data on shutdown.

- `DataManager` (`DataManager.h` / `DataManager.cpp`)
  - Model/persistence layer. Loads/saves data from `saves/` and exposes APIs for windows to read/write wallets, transactions, categories, and recurring entries.

- Screens / Windows
  - `dashboard` — top-level overview and navigation (wallet summaries, buttons to open other screens).
  - `TransactionWindow` — create/edit income & expense transactions (forms, validation).
  - `RecurringWindow` — manage recurring transactions and scheduling.
  - `MasterDataWindow` — CRUD for wallets, income sources, and expense categories.

- Reusable widgets (headers & source in `GUI/include` / `GUI/src`)
  - `button.h/cpp` — clickable button control.
  - `Dropdown.h/cpp` — selection control.
  - `TextInput.h/cpp` — single-line text input control.
  - `ScrollArea.h/cpp` — scrollable container for long content.
  - `Dialog.h/cpp` — modal dialogs and confirmations.

- Helpers
  - `Helper.h/cpp` and `LayoutHelper.h/cpp` — drawing utilities, formatting, and layout helpers.

## Prebuilt binary (packaged)
A macOS prebuilt executable and zip archive are available in the repository `prebuild/` folder:

- `prebuild/PersonalFinanceManager` — native macOS executable (moved here after build)
- `prebuild/PersonalFinanceManager-macos.zip` — zip containing the executable

To run the prebuilt binary on macOS:

```bash
# from repository root
./prebuild/PersonalFinanceManager
# or unzip and run
unzip prebuild/PersonalFinanceManager-macos.zip -d /tmp/pfm && /tmp/pfm/PersonalFinanceManager
```

Notes:
- The prebuilt binary targets macOS and is a UI application (opens a raylib window).
- The program reads/writes persistent data in the `saves/` folder. If you want a fresh start, remove or move the `saves/` files.

## Running on Windows (prebuilt .exe)
A Windows build exist under `build/Debug/PersonalFinanceManager.exe` 

## Building from source (macOS)
Prerequisites
- Homebrew, CMake, pkg-config, raylib

Install prerequisites:

```bash
brew update
brew install cmake pkg-config raylib
```

Configure and build (recommended out-of-source build):

```bash
# from repo root
mkdir -p build && cd build
# make sure pkg-config can find raylib; on Apple Silicon Homebrew path is included above
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(sysctl -n hw.ncpu)
```

The built executable will typically be `build/PersonalFinanceManager`.

## Common troubleshooting
- CMake re-uses a `CMakeCache.txt` in the build folder. If you see platform-specific artifacts (Visual Studio files or a Windows build), remove the old `build/` directory before configuring for your platform.

```bash
rm -rf build && mkdir build
```

- If CMake cannot find raylib, check `pkg-config`:

```bash
pkg-config --cflags raylib
pkg-config --libs raylib
```

and set `PKG_CONFIG_PATH` appropriately if necessary.

- Case-sensitive include warnings: filenames and include statements must match exactly on case-sensitive file systems. The repository was updated to use lowercase `button.h` includes.

- Avoid running GUI builds inside cloud-synced directories (Dropbox/Google Drive) to prevent cross-platform cache conflicts.

## Quick developer tips
- To add a headless test target, consider adding a CLI target that builds `src/mainCLI.cpp` as a separate executable.
- Keep headers free of `using namespace std;` to avoid leaking names to all translation units.
- Use consistent file naming (lowercase headers) to avoid portability issues.

## Where to find more
- GUI sources are under `GUI/` (headers in `GUI/include`, implementation in `GUI/src`).
- Core model code is under `src/` (date, wallet, transactions, statistic, recurring).

If you'd like, I can add a short CONTRIBUTING.md with precise platform instructions or create a GitHub Actions CI workflow that builds the CLI target for automated tests.
